"""
导出模型为onnx模型，人工撰写
"""
from DQNAgent import Agent,Agent1
from Match3_Env import Match3Env
import torch

env = Match3Env()
state,_ = env.reset()
agent = Agent(input_shape=(3,4,4),n_action=24)
# agent = Agent1(input_shape=(3,4,4),n_action=24)

device = torch.device("cuda") if torch.cuda.is_available() \
        else torch.device("cpu")

agent.policy_net.load_state_dict(torch.load("match3_dqn_model_v4.pth", weights_only=True))

agent.policy_net.eval()

state_tensor = agent.process(state).to(device).unsqueeze(0)

torch.onnx.export(
    agent.policy_net,
    state_tensor,
    f="CnnDQN_for_Match3.onnx",
    input_names=["input"], # 输入命名
    output_names=["output"], # 输出命名
    dynamic_axes={'input':{0:'batch'}, 'output':{0:'batch'}}
)