"""
收集状态输入作为校准数据集，收集Q值输出可作为验证，主要由人工撰写，少量gemini 2.5pro代码
"""
import numpy as np
from DQNAgent import  Agent
from Match3_Env import Match3Env
import torch
import random


device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
INPUT_SIZE = (3,4,4)
env = Match3Env()
agent = Agent(INPUT_SIZE,n_action=24)


total_steps = 0
samples = []
Q_values = []

agent.epsilon = 0.05
agent.policy_net.load_state_dict(torch.load("match3_dqn_model_v4.pth", weights_only=True))


for i in range(150):
    state, _ = env.reset()
    done = False
    while not done:
        state = agent.process(state).unsqueeze(0).to(device)
        q = agent.policy_net(state)

        state1 = state.cpu().numpy().astype(np.float32)
        q1 = q.cpu().detach().numpy().astype(np.float32)

        samples.append(state1)
        Q_values.append(q1)
        if random.random() < agent.epsilon:
            # 随机动作，以增强覆盖
            action = random.randint(0,23)
        else:
            action = int(torch.argmax(q, dim=1).cpu().item())

        next_state, reward, done, _, _ = env.step(action)
        state = next_state

np.savez("sample_fixed.npz", *samples)
np.savez("Qval_fixed.npz", *Q_values)
print("Saved sample_fixed.npz ({} samples) and Qval_fixed.npz ({} samples)".format(len(samples), len(Q_values)))
