"""
该部分主要由gemini AI 2.5pro生成，人工轻微修改
"""
import gymnasium as gym
from gymnasium import spaces
import numpy as np
import time


class Match3Env(gym.Env):
    """
    一个4x4的三消棋类环境 (Match-3)
    - 状态(Observation): 4x4的棋盘, 元素值为 1, 2, 3。
    - 动作(Action): 离散值，代表交换相邻格子的操作。
      动作总数 = (4 * 3) + (4 * 3) = 24 (12个水平交换 + 12个垂直交换)
    - 奖励(Reward): 原本为每次消除的元素数量的平方，但为了解决梯度爆炸问题，进行了奖励裁剪。
    """
    metadata = {'render.modes': ['human']}

    def __init__(self, board_size=4, num_elements=3, max_steps=30):
        super(Match3Env, self).__init__()
        self.board_size = board_size
        self.num_elements = num_elements
        self.max_steps = max_steps
        self.current_step = 0

        # 状态空间: 4x4的棋盘，每个格子的值在[1, num_elements]
        self.observation_space = spaces.Box(low=1, high=num_elements,
                                            shape=(self.board_size, self.board_size), dtype=np.int32)

        # 动作空间: 24个可能的交换动作
        # 0-11: 水平交换 (row, col) 和 (row, col+1)
        # 12-23: 垂直交换 (row, col) 和 (row+1, col)
        self.action_space = spaces.Discrete((self.board_size * (self.board_size - 1)) * 2)

    def _get_obs(self):
        return self.board.copy()

    def reset(self, seed=None, options=None):
        super().reset(seed=seed)
        self.board = self.np_random.integers(1, self.num_elements+1, size=(self.board_size, self.board_size))
        # 确保初始棋盘没有可消除的组合
        while self._find_matches()[0]:
            self._resolve_matches()
        self.current_step = 0
        return self._get_obs(), {}

    def step(self, action):
        self.current_step += 1

        # 1. 执行动作 (交换)
        is_valid_swap = self._swap(action)
        if not is_valid_swap:
            # 如果是非法交换（比如边界），给予惩罚并结束
            return self._get_obs(), -10.0, True, False, {"error": "Invalid swap"}

        total_reward = 0
        combo_multiplier = 1.0

        # 2. 循环检测和处理消除
        while True:
            matches_found, matched_board = self._find_matches()
            if not matches_found:
                break  # 没有可消除的了，退出循环

            # 3. 计算奖励
            num_eliminated = np.sum(matched_board)
            reward = num_eliminated * combo_multiplier
            total_reward += reward

            # 4. 消除、下落、补充
            self._resolve_matches(matched_board)

            combo_multiplier = 0.15 # 连锁消除奖励系数

        # 如果一步操作后没有任何消除，给予一个小小的负奖励鼓励有效操作
        if total_reward == 0:
            total_reward = -1.0

        # 5. 检查是否结束
        done = self.current_step >= self.max_steps

        return self._get_obs(),float(total_reward), done, False, {}

    def _swap(self, action):
        board_size = self.board_size
        # 解析动作
        # 0-11 为水平交换动作
        if 0 <= action < board_size * (board_size - 1):
            row = action // (board_size - 1)
            col = action % (board_size - 1)
            # Swap (row, col) and (row, col+1)
            self.board[row, col], self.board[row, col + 1] = self.board[row, col + 1], self.board[row, col]
        # 12-23 为垂直交换动作
        elif board_size * (board_size - 1) <= action < (board_size * (board_size - 1)) * 2:
            action_rem = action - (board_size * (board_size - 1))
            row = action_rem // board_size
            col = action_rem % board_size
            # 执行交换
            self.board[row, col], self.board[row + 1, col] = self.board[row + 1, col], self.board[row, col]
        else:
            return False  # 无效动作
        return True

    def _find_matches(self):
        matched_board = np.zeros_like(self.board, dtype=bool)

        # 检查水平方向
        for r in range(self.board_size):
            for c in range(self.board_size - 2):
                if self.board[r, c] == self.board[r, c + 1] == self.board[r, c + 2]:
                    matched_board[r, c:c + 3] = True

        # 检查垂直方向
        for c in range(self.board_size):
            for r in range(self.board_size - 2):
                if self.board[r, c] == self.board[r + 1, c] == self.board[r + 2, c]:
                    matched_board[r:r + 3, c] = True

        return np.any(matched_board), matched_board

    def _resolve_matches(self, matched_board=None):
        if matched_board is None:
            _, matched_board = self._find_matches()

        # 将匹配的元素设置为空 (-1)
        self.board[matched_board] = -1

        # 重力下落
        for c in range(self.board_size):
            empty_count = 0
            for r in range(self.board_size - 1, -1, -1):
                if self.board[r, c] == -1:
                    empty_count += 1
                elif empty_count > 0:
                    self.board[r + empty_count, c] = self.board[r, c]
                    self.board[r, c] = -1

        # 随机补充顶部
        for c in range(self.board_size):
            for r in range(self.board_size):
                if self.board[r, c] == -1:
                    self.board[r, c] = self.np_random.integers(1, self.num_elements+1)

    def render(self, mode='human'):
        if mode == 'human':
            element_map = {1: '🍎', 2: '🍌', 3: '🍇', -1: '  '}
            print("\033c", end="")  # 清屏
            print(f"Step: {self.current_step}/{self.max_steps}")
            print("┌" + "───┬" * (self.board_size - 1) + "───┐")
            for r in range(self.board_size):
                row_str = "│ "
                for c in range(self.board_size):
                    row_str += f"{element_map[self.board[r, c]]} │ "
                print(row_str)
                if r < self.board_size - 1:
                    print("├" + "───┼" * (self.board_size - 1) + "───┤")
            print("└" + "───┴" * (self.board_size - 1) + "───┘")
            time.sleep(0.5)
