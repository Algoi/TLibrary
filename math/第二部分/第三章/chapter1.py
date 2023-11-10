import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

# 创建一个3D坐标轴
fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')

# 坐标点的位置
# x, y, z = -1, -2, 2
x = [-1, 3]
y = [-2, 4]
z = [2, 3]

# 绘制坐标点
# ax.scatter(x, y, z, color='red', label=f'Point ({x}, {y}, {z})')

# 在坐标点位置显示坐标信息
ax.text(x[0], y[0], z[0], f'({x[0]}, {y[0]}, {z[0]})', color='black', fontsize=10, bbox=dict(facecolor='white', alpha=0.7))
ax.text(x[1], y[1], z[1], f'({x[1]}, {y[1]}, {z[1]})', color='black', fontsize=10, bbox=dict(facecolor='white', alpha=0.7))

# 绘制线段
ax.plot(x, y, z, color='blue', label='Line Segment')

# 设置坐标轴标签
ax.set_xlabel('X')
ax.set_ylabel('Y')
ax.set_zlabel('Z')

# 设置图例
ax.legend()

# 显示图形
plt.show()
