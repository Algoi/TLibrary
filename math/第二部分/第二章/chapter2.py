import matplotlib.pyplot as plt

# 将坐标点分解为x和y坐标列表
x_coordinates = [i for i in range(-10, 11)]
y_coordinates = [point**2 for point in x_coordinates]

# 使用plot函数将这些点连接起来
plt.plot(x_coordinates, y_coordinates)

# 显示图形
plt.show()

print(x_coordinates)
