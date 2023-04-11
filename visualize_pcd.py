import numpy as np
import open3d as o3d

root = "./pointcloud/" # user input
file_num = 19
points = np.loadtxt(root+str(file_num)+".txt", delimiter=",")

pcd = o3d.geometry.PointCloud()
pcd.points = o3d.utility.Vector3dVector(points)
o3d.visualization.draw_geometries([pcd],
                                  zoom=0.9,
                                  front=[-1, 2.5, 2],
                                  lookat=[0, 0, 40],
                                  up=[0, 0, 1],)