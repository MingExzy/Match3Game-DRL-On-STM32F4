"""
该部分代码主要由chat-gpt-5.2撰写，微量人工修改
"""
from collections import deque
import numpy as np

class QuantileRewardClipper:
    """
    自适应的分位数奖励裁剪与缩放，自适应裁剪，用窗口内的95%分位数作为奖励上界，
    自适应地裁剪正奖励并按该上界缩放到一个目标正值 pos_target（默认 10）。
    对负奖励（r<=0）使用固定下界 neg_clip（默认 -1.0）裁剪。
    """

    def __init__(
        self,
        q: float = 0.95,
        window_size: int = 5000,
        update_every: int = 1000,
        neg_clip: float = -1.0,
        r_pos_min: float = 1.0,
        r_pos_max: float = 1e6,
        pos_target: float = 10.0,
        scale_positive: bool = True,
        eps: float = 1e-8,
    ):
        assert 0.5 <= q < 1.0
        self.q = q
        self.window_pos = deque(maxlen=window_size)
        self.update_every = update_every

        self.neg_clip = float(neg_clip)
        self.R_pos_min = float(r_pos_min)
        self.R_pos_max = float(r_pos_max)

        self.pos_target = float(pos_target)
        self.scale_positive = bool(scale_positive)

        self.eps = float(eps)
        self._t = 0
        self.R_pos = self.R_pos_min

    def observe(self, r: float):
        self._t += 1
        r = float(r)
        if r > 0:
            self.window_pos.append(r)

        if (self._t % self.update_every == 0) and (len(self.window_pos) >= 500):
            arr = np.asarray(self.window_pos, dtype=np.float32)
            R = float(np.quantile(arr, self.q))
            R = min(max(R, self.R_pos_min), self.R_pos_max)
            self.R_pos = R

    def transform(self, r: float) -> float:
        r = float(r)
        if r <= 0.0:
            return max(self.neg_clip, r)
        R = max(self.R_pos, self.R_pos_min)
        r = min(r, R)
        if self.scale_positive:
            return (r / (R + self.eps)) * self.pos_target
        else:
            return r