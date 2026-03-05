"""
该部分代码主要由gemini 2.5pro生成，人工轻微修改，
用于训练和测试智能体
"""
from Match3_Env import Match3Env
from DQNAgent import Agent
import numpy as np
import torch
import math
import matplotlib.pyplot as plt

# --- 超参数 ---
EPISODES = 25000
EPS_START = 1.0
EPS_END = 0.01
EPS_DECAY = EPISODES/3
TARGET_UPDATE_FREQ = 100# 每100步更新一次目标网络
INPUT_SIZE = (3,4,4)

# --- 初始化 ---
env = Match3Env()
agent = Agent(INPUT_SIZE,n_action=24)

epsilon = EPS_START
total_steps = 0
all_rewards = []
returns_list = []

# # --- 训练循环 ---
for episode in range(EPISODES):
    state, _ = env.reset()
    episode_reward = 0
    done = False

    while not done:

        # 前1000回合纯随机增加探索性
        if episode<1000:
            epsilon=1
        else:
            epsilon = EPS_END + (EPS_START - EPS_END) * math.exp(-1. * (episode-1000) / EPS_DECAY)
        agent.epsilon =epsilon

        # 1.选择动作
        action = agent.select_action(state)
        next_state, reward, done, _, info = env.step(action)

        # 2. 存储经验到回放缓冲
        state_tensor = agent.process(state)
        next_state_tensor = agent.process(next_state)
        agent.memory.push(state_tensor, action, reward, next_state_tensor, done)

        # 3. 更新状态和奖励
        state = next_state
        episode_reward += reward
        total_steps += 1

        # 4. 训练模型
        loss = agent.update_model()
        # print("损失函数值为:",loss)

        # 5. 定期更新目标网络
        if total_steps % TARGET_UPDATE_FREQ == 0:
            agent.update_target_net()

    all_rewards.append(episode_reward)
    if (episode+1)%100 == 0:
        avg_reward = np.mean(all_rewards[-100:])
        returns_list.append(avg_reward)

        print(
            f"Episode {episode + 1}/{EPISODES}, Steps: {total_steps}, Reward: {episode_reward:.2f}, Avg Reward(100): {avg_reward:.2f}, Epsilon: {epsilon:.3f}")

# 训练结束后可以保存模型
episode = [(i+1)*100 for i in range(len(returns_list))]
plt.figure()
plt.xlabel("episode")
plt.ylabel("avg_reward")
plt.plot(episode,returns_list,label='avg_reward_curve')
plt.legend()
plt.show()

torch.save(agent.policy_net.state_dict(), "match3_dqn_model_.pth")
print("训练完成!")

# --- 测试训练好的智能体 ---
print("\n--- 开始测试100回合 ---")
agent.policy_net.load_state_dict(torch.load("match3_dqn_model_.pth", weights_only=True))
reward_list =[]
for i in range(100):
    state, _ = env.reset()
    done = False
    total_test_reward = 0
    agent.epsilon = 0.0
    while not done:
        env.render()
        action = agent.select_action(state) # 使用贪心策略
        print(action)
        next_state, reward, done, _, _ = env.step(action)
        state = next_state
        total_test_reward += reward
    reward_list.append(total_test_reward)

print(f"测试结束, 100回合平均奖励: {sum(reward_list)/len(reward_list)}")
