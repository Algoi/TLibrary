import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

# 创建一个3D坐标轴
fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')

# 用于向量相加，接收一条线段和增量
def addVector(origin, increament):
    return [p1 + p2 for p1, p2 in zip(origin, increament)]

x1 = [-1, 3]
y1 = [-2, 4]
z1 = [2, 3]

x2 = [1, 4]
y2 = [2, 5]
z2 = [4, 8]

ax.plot(x1, y1, z1, color='blue', label='Line Segment1')
ax.plot(x2, y2, z2, color='red', label='Line Segment2')

resultx = addVector(x1, x2)
resulty = addVector(y1, y2)
resultz = addVector(z1, z2)

ax.plot(resultx, resulty, resultz, color='green', label='Line Segment3')

print(resultx, resulty, resultz)

# 设置图例
ax.legend()

# 显示图形
plt.show()

