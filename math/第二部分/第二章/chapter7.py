import matplotlib.pyplot as plt
import math

# 将笛卡尔坐标列表转为极坐标
def to_polar(v):
    x, y = v[0], v[1]
    len = math.sqrt(x ** 2 + y ** 2)
    polar = math.atan2(y, x)
    return [len, polar]

def to_polars(vls):
    return [to_polar(point) for point in vls]

# 将极坐标转为笛卡尔坐标
def to_cartesian(pol):
    len, polar = pol[0], pol[1]
    return [len * math.cos(polar), len * math.sin(polar)]

def to_cartesians(pols):
    return [to_cartesian(point) for point in pols]

# 对角度进行加减操作
def to_rotate(pol, angle):
    return [pol[0], pol[1] + angle * math.pi / 180]

def to_rotates(pols, angle):
    return [to_rotate(point, angle) for point in pols]

# 平移
def to_move(v, devx, devy):
    return [v[0] + devx, v[1] + devy]

def to_moves(vls, devx, devy):
    return [to_move(v, devx, devy) for v in vls]

# 缩放
def to_scale(v, u):
    return [v[0] * u, v[1] * u]

def to_scales(vls, u):
    return [to_scale(p, u) for p in vls]

# 平移旋转
def to_dev_rotate(v, dev, angle):
    # 先将v转为极坐标
    pol_v = to_polar(v)
    rotateLP = to_rotate(pol_v, angle)  # 旋转
    # 转为笛卡尔坐标
    rotateLD = to_cartesian(rotateLP)

    # 平移
    result = to_move(rotateLD, dev[0], dev[1])
    return result

def to_dev_rotates(vls, dev, angle):
    return [to_dev_rotate(v, dev, angle) for v in vls]

# 旋转恐龙
# 定义恐龙形状的坐标点
points = [(6,4), (3,1), (1,2), (-1,5), (-2,5), (-3,4), (-4,4), 
 (-5,3), (-5,2), (-2,2), (-5,1), (-4,0), (-2,1), (-1,0), (0,-3),  
 (-1,-4), (1,-4), (2,-3), (1,-2), (3,-1), (5,1), (6,4)]

# 将坐标点分解为x和y坐标列表
x_coordinates = [point[0] for point in points]
y_coordinates = [point[1] for point in points]

# 使用plot函数将这些点连接起来
plt.plot(x_coordinates, y_coordinates)

devRota = to_dev_rotates(points, [1, 2], -90)

plt.plot([point[0] for point in devRota], [point[1] for point in devRota], color="yellow")

# 显示图形
plt.show()
