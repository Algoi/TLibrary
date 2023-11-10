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

# 一、开始旋转
# 1、先转为极坐标
points_rotate = to_polars(points)
# 2、旋转
points_rd = to_rotates(points_rotate, 45)
# 3、转为笛卡尔坐标
points_cd = to_cartesians(points_rd)

x_r_c = [p[0] for p in points_cd]
y_r_c = [p[1] for p in points_cd]
plt.plot(x_r_c, y_r_c, color="red")

# 二、平移
def to_move(v, devx, devy):
    return [v[0] + devx, v[1] + devy]

def to_moves(vls, devx, devy):
    return [to_move(v, devx, devy) for v in vls]

locations_dev = to_moves(points, 1, 1)
plt.plot([point[0] for point in locations_dev], [point[1] for point in locations_dev], color = "gray")

# 三、缩放
def to_scale(v, u):
    return [v[0] * u, v[1] * u]

def to_scales(vls, u):
    return [to_scale(p, u) for p in vls]

locations_scale = to_scales(points, 1.5)  # 扩大1.5倍
plt.plot([point[0] for point in locations_scale], [point[1] for point in locations_scale], color = "yellow")

####################################################
# 在原图形上添加标注
plt.annotate('origin', xy=(x_coordinates[0], y_coordinates[0]), xytext=(x_coordinates[0], y_coordinates[0] + 2),
             arrowprops=dict(facecolor='black', shrink=0.05))

# 在旋转的图形上添加标注
plt.annotate('rotate', xy=(x_r_c[0], y_r_c[0]), xytext=(x_r_c[0], y_r_c[0] + 2),
             arrowprops=dict(facecolor='red', shrink=0.05))

# 在平移的图形上添加标注
plt.annotate('dev', xy=(locations_dev[0][0], locations_dev[0][1]), xytext=(locations_dev[0][0], locations_dev[0][1] + 2),
             arrowprops=dict(facecolor='gray', shrink=0.05))

# 在缩放的图形上添加标注
plt.annotate('scale', xy=(locations_scale[0][0], locations_scale[0][1]), xytext=(locations_scale[0][0], locations_scale[0][1] + 2),
             arrowprops=dict(facecolor='yellow', shrink=0.05))

################################################################
# 显示图形
plt.show()
