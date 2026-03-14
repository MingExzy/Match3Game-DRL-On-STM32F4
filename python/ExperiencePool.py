"""
该部分代码中优先经验回放以及SumTree由Chat-gpt-5.2生成，其他由人工撰写
"""
from collections import deque
from typing import Tuple
import random
import numpy as np


class ReplayBuffer:
    def __init__(self, capacity:int):
        self.buffer = deque(maxlen=capacity)

    def add(self, ex):
        """
        添加经验
        :param ex = (state, action, reward, next_state, done) 状态，动作，奖励，下一状态，结束标志
        :return:
        """
        self.buffer.append(ex)

    def sample(self, batch_size):
        """
        随机采样
        :param batch_size: 经验批次
        :return: 随机采样的经验
        """
        states, actions, rewards, next_states, dones = zip(*random.sample(self.buffer, batch_size))

        return states, actions, rewards, next_states, dones

    def __len__(self):
        return len(self.buffer)


class SumTree:
    """
    用sumTree存储经验
    """

    def __init__(self,capacity:int):
        self.capacity = capacity
        self.tree = np.zeros(2*capacity - 1, dtype=np.float32) #用数组存储树节点，节点值表示优先级
        self.experience = np.empty(capacity,dtype=tuple) #存储经验
        self.point = 0
        self.size = 0

    @property
    def total(self) -> float:
        return float(self.tree[0])

    def add(self,experience:tuple,priority:float):
        index = self.point
        self.experience[index] = experience
        self.update(index,priority)
        self.point = (self.point+1)%self.capacity #当前数据指针
        self.size = min(self.size+1,self.capacity) #当前所以容纳的数据

    def update(self,index:int,priority:float):

        if (not np.isfinite(priority)) or priority <= 0.0:
            priority = 1e-6
        tree_node = index + self.capacity - 1
        delta = priority - self.tree[tree_node]
        self.tree[tree_node] = priority

        while tree_node: #从下而上更新变化值，以方便更新根节点的值（用于后续提取时用）
            tree_node = (tree_node-1)//2
            self.tree[tree_node]+=delta


    def get(self, s: float) -> Tuple[int, float, tuple]:

        idx = 0
        while True:
            left = 2 * idx + 1
            right = left + 1
            if left >= len(self.tree):
                break
            if s <= self.tree[left]:
                idx = left
            else:
                s -= self.tree[left]
                idx = right

        data_index = idx - (self.capacity - 1)
        return data_index, float(self.tree[idx]), self.experience[data_index]


class PrioritizedReplayBuffer:
    """
    用sumTree实现优先经验回放
    """

    def __init__(
        self,
        capacity: int,
        alpha: float = 0.6,
        beta: float = 0.4,
        beta_increment_per_sampling: float = 1e-4,
        eps: float = 1e-3,
    ):
        self.alpha = alpha
        self.beta = beta
        self.beta_increment_per_sampling = beta_increment_per_sampling
        self.eps = eps

        self.tree = SumTree(capacity)
        self.max_priority = 1.0  # new transitions start with max priority

    def __len__(self) -> int:
        return self.tree.size

    def _priority(self, td_error: float) -> float:
        return float((abs(td_error) + self.eps) ** self.alpha)

    def add(self, experience, td_error: float | None = None):
        """
        添加经验（s,a,r,next_s,done)，采用延迟计算，对位计算td_error的样本给予最高优先级
        """
        if td_error is None:
            p = self.max_priority
        else:
            p = self._priority(td_error)
            self.max_priority = max(self.max_priority, p)
        self.tree.add(experience,p)

    def sample(self, batch_size: int):
        """
        优先经验采样，当树未满时选择随机均匀采样，避免出现采样到未更新的树节点（优先值为0，对应经验为None）
        """
        if len(self) < batch_size:
            return None
        if len(self) < self.tree.size:
            batch = []
            indices = np.empty(batch_size, dtype=np.int64)
            for i in range(batch_size):
                idx = random.randint(0,self.tree.size)
                indices[i] = idx
                data = self.tree.experience[idx]
                batch.append(data)

            is_weights = np.ones(batch_size,dtype=np.float32)
            return batch, indices, is_weights

        self.beta = min(1.0, self.beta + self.beta_increment_per_sampling)

        batch  = []
        indices = np.empty(batch_size, dtype=np.int64)
        priorities = np.empty(batch_size, dtype=np.float32)

        total = self.tree.total
        segment = total / batch_size
        for i in range(batch_size):
            a = segment * i
            b = segment * (i + 1)
            s = random.random()*(b-a)+a
            idx, p, data = self.tree.get(s)
            batch.append(data)
            indices[i] = idx
            priorities[i] = p
        probs = priorities / total
        N = len(self)
        is_weights = (N * probs) ** (-self.beta)
        is_weights /= is_weights.max()
        return batch, indices, is_weights.astype(np.float32)

    def update_priorities(self, indices: list[int], td_errors: list[float]):
        """
        更新优先级
        """
        assert len(indices) == len(td_errors)
        for idx, err in zip(indices, td_errors):
            p = self._priority(float(err))
            self.tree.update(int(idx), p)
            self.max_priority = max(self.max_priority, p)







