import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

# 创建一个3D坐标轴
fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')

# 用于向量相加，接收一条线段和增量
def addVector(origin, increament):
    return [p1 + p2 for p1, p2 in zip(origin, increament)]

# 向量乘法
def multiVector(origin, u):
    return [x*u for x in origin]

p1 = [0, 0, 0]
p2 = [2, 4, 6]

ax.plot([p1[0], p2[0]], [p1[1], p2[1]], [p1[2], p2[2]], color='red', linewidth=3, label='Line Segment1')

p1m = multiVector(p1, -2)
p2m = multiVector(p2, -2)
ax.plot([p1m[0], p2m[0]], [p1m[1], p2m[1]], [p1m[2], p2m[2]], color='green', label='Line Segment2')

# 设置坐标轴标签
ax.set_xlabel('X')
ax.set_ylabel('Y')
ax.set_zlabel('Z')

# 显示图形
plt.show()

