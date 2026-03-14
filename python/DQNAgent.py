"""
该部分代码主要由gemini 2.5pro生成， 人工轻微修改
"""
import torch
import torch.nn as nn
import torch.optim as optim
import random
import numpy as np
from ExperiencePool import ReplayBuffer,PrioritizedReplayBuffer


class DQN(nn.Module):
    """
    DQN网络: CNN提取特征 + MLP输出Q值
    输入: (N, C, H, W) =  (N, 3, 4, 4)
    """

    def __init__(self, input_shape, n_actions):
        super(DQN, self).__init__()

        C,H,W = input_shape[0],input_shape[1],input_shape[2]

        self.cnn_base = nn.Sequential(
            nn.Conv2d(C, 32, kernel_size=3, stride=1, padding=1),
            nn.ReLU(),
            nn.Conv2d(32, 64, kernel_size=3, stride=1, padding=1),
            nn.ReLU()
        )

        self.mlp_base = nn.Sequential(
            nn.Flatten(),
            nn.Linear(64*H*W, 256),
            nn.ReLU(),
            nn.Linear(256, n_actions)
        )


    def forward(self, x):
        conv_out = self.cnn_base(x) # 展平
        return self.mlp_base(conv_out)


class Agent:
    def __init__(self, input_shape, n_action,buffer_capacity=10000, lr=1e-5, batch_size=32, gamma=0.99,epsilon=0.1):
        self.batch_size = batch_size
        self.gamma = gamma
        self.device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
        self.epsilon = epsilon

        self.policy_net = DQN(input_shape, n_action).to(self.device)
        self.target_net = DQN(input_shape, n_action).to(self.device)
        self.target_net.load_state_dict(self.policy_net.state_dict())
        self.target_net.eval()

        self.optimizer = optim.Adam(self.policy_net.parameters(), lr=lr)
        self.memory = ReplayBuffer(buffer_capacity)

    def select_action(self, state):
        """
        动作选择函数，epsilon-greedy策略
        :param state: (1,3,4,4)的张量
        :return: 动作索引
        """
        if random.random() > self.epsilon:
            with torch.no_grad():
                state_tensor = self.process(state).to(self.device).unsqueeze(0)
                q_values = self.policy_net(state_tensor).squeeze(0)
                action = q_values.max(0)[1]
                action = action.cpu()
                action = action.item()
                return action
        else:
            return random.randint(0,23)

    def update_model(self):
        """
        模型更新
        :return:
        """
        if len(self.memory) < self.batch_size:
            return
        states, actions, rewards, next_states, dones = self.memory.sample(self.batch_size)


        states_t = torch.stack(states,dim=0).to(self.device)
        actions_t = torch.tensor(actions).to(self.device).view(-1,1)
        rewards_t = torch.tensor(rewards, dtype=torch.float32).to(self.device).view(-1,1)
        next_states_t = torch.stack(next_states,dim=0).to(self.device)
        dones_t = torch.tensor(dones,dtype=torch.float).to(self.device).view(-1,1)

        # 计算当前状态的Q值
        q_values = self.policy_net(states_t).gather(1, actions_t)

        # 计算下一个状态的Q值 (使用目标网络)
        next_q_values = self.target_net(next_states_t).max(1)[0].view(-1,1)

        # 计算期望Q值
        expected_q_values = rewards_t + self.gamma * next_q_values*(1-dones_t)

        # 计算损失
        loss = nn.MSELoss(reduction="mean")(q_values, expected_q_values)

        # 优化模型
        self.optimizer.zero_grad()
        loss.backward()
        torch.nn.utils.clip_grad_norm_(self.policy_net.parameters(), max_norm=10.0)

        # print("--- 梯度范数检查 ---")
        # for name, p in self.policy_net.named_parameters():
        #     if p.grad is not None:
        #         param_norm = p.grad.data.norm(2)
        #         print(f"层: {name:<20} | 梯度范数: {param_norm.item():.4f}")
        #     else:
        #         print(f"层: {name:<20} | 梯度: None (该层没有梯度)")

        self.optimizer.step()

        return loss.item()

    def update_target_net(self):
        """
        更新目标网络
        :return:
        """
        self.target_net.load_state_dict(self.policy_net.state_dict())

    @staticmethod
    def process(board:np.ndarray):
        """
        数据处理函数，基于numpy数组的board生成(1,3,4,4)(N,C,H,W)的张量
        :param board: numpy数组
        :return: (1,3,4,4)形状的张量
        """
        board = torch.tensor(board,dtype=torch.float)
        n = board.size()[0]
        pos1 = torch.nonzero(board == 1)
        pos2 = torch.nonzero(board == 2)
        pos3 = torch.nonzero(board == 3)

        board1 = torch.zeros((n, n))
        board2 = torch.zeros((n, n))
        board3 = torch.zeros((n, n))

        board1[pos1[:,0],pos1[:,1]] = 1
        board2[pos2[:, 0], pos2[:, 1]] = 2
        board3[pos3[:, 0], pos3[:, 1]] = 3

        board_tensor = torch.stack([board1,board2,board3],dim=0)

        return board_tensor

class Agent1:
    def __init__(self, input_shape, n_action,buffer_capacity=10000, lr=1e-5, batch_size=32, gamma=0.99,epsilon=0.1):
        self.batch_size = batch_size
        self.gamma = gamma
        self.device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
        self.epsilon = epsilon

        self.policy_net = DQN(input_shape, n_action).to(self.device)
        self.target_net = DQN(input_shape, n_action).to(self.device)
        self.target_net.load_state_dict(self.policy_net.state_dict())
        self.target_net.eval()

        self.optimizer = optim.Adam(self.policy_net.parameters(), lr=lr)
        self.memory = PrioritizedReplayBuffer(buffer_capacity)

    def select_action(self, state):
        """
        动作选择函数，epsilon-greedy策略
        :param state: (1,3,4,4)的张量
        :return: 动作索引
        """
        if random.random() > self.epsilon:
            with torch.no_grad():
                state_tensor = self.process(state).to(self.device).unsqueeze(0)
                q_values = self.policy_net(state_tensor).squeeze(0)
                action = q_values.max(0)[1]
                action = action.cpu()
                action = action.item()
                return action
        else:
            return random.randint(0,23)

    def update_model(self):
        """
        模型更新
        :return:
        """
        if len(self.memory) < self.batch_size:
            return
        batch, indices, is_weights = self.memory.sample(self.batch_size)

        states,actions,rewards,next_states,dones = zip(*batch)

        states_t = torch.stack(states,dim=0).to(self.device)
        actions_t = torch.tensor(actions).to(self.device).view(-1,1)
        rewards_t = torch.tensor(rewards, dtype=torch.float32).to(self.device).view(-1,1)
        next_states_t = torch.stack(next_states,dim=0).to(self.device)
        dones_t = torch.tensor(dones,dtype=torch.float).to(self.device).view(-1,1)
        is_weights = torch.tensor(is_weights,dtype=torch.float).to(self.device).view(-1,1)

        # 计算当前状态的Q值
        q_values = self.policy_net(states_t).gather(1, actions_t)

        # 计算下一个状态的Q值 (使用目标网络)
        next_q_values = self.target_net(next_states_t).max(1)[0].view(-1,1)

        # 计算期望Q值
        expected_q_values = rewards_t + self.gamma * next_q_values*(1-dones_t)
        td_error = (expected_q_values - q_values).detach().cpu()
        td_error = [i.item() for i in td_error]


        # 计算损失
        loss = nn.MSELoss(reduction="none")(q_values,expected_q_values)
        loss = (is_weights.detach()*loss).mean()

        # 优化模型
        self.optimizer.zero_grad()
        loss.backward()
        torch.nn.utils.clip_grad_norm_(self.policy_net.parameters(), max_norm=10.0)

        # print("--- 梯度范数检查 ---")
        # for name, p in self.policy_net.named_parameters():
        #     if p.grad is not None:
        #         param_norm = p.grad.data.norm(2)
        #         print(f"层: {name:<20} | 梯度范数: {param_norm.item():.4f}")
        #     else:
        #         print(f"层: {name:<20} | 梯度: None (该层没有梯度)")

        self.optimizer.step()

        self.memory.update_priorities(indices,td_error)

        return loss.item()

    def update_target_net(self):
        """
        更新目标网络
        :return:
        """
        self.target_net.load_state_dict(self.policy_net.state_dict())

    @staticmethod
    def process(board:np.ndarray):
        """
        数据处理函数，基于numpy数组的board生成(1,3,4,4)(N,C,H,W)的张量
        :param board: numpy数组
        :return: (1,3,4,4)形状的张量
        """
        board = torch.tensor(board,dtype=torch.float)
        n = board.size()[0]
        pos1 = torch.nonzero(board == 1)
        pos2 = torch.nonzero(board == 2)
        pos3 = torch.nonzero(board == 3)

        board1 = torch.zeros((n, n))
        board2 = torch.zeros((n, n))
        board3 = torch.zeros((n, n))

        board1[pos1[:,0],pos1[:,1]] = 1
        board2[pos2[:, 0], pos2[:, 1]] = 2
        board3[pos3[:, 0], pos3[:, 1]] = 3

        board_tensor = torch.stack([board1,board2,board3],dim=0)

        return board_tensor
