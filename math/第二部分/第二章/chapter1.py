import matplotlib.pyplot as plt

# 定义恐龙形状的坐标点
points = [(6,4), (3,1), (1,2), (-1,5), (-2,5), (-3,4), (-4,4), 
 (-5,3), (-5,2), (-2,2), (-5,1), (-4,0), (-2,1), (-1,0), (0,-3),  
 (-1,-4), (1,-4), (2,-3), (1,-2), (3,-1), (5,1), (6,4)]

# 将坐标点分解为x和y坐标列表
x_coordinates = [point[0] for point in points]
y_coordinates = [point[1] for point in points]

# 使用plot函数将这些点连接起来
plt.plot(x_coordinates, y_coordinates)

# 显示图形
plt.show()
