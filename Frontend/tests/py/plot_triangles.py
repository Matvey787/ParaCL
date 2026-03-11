import matplotlib.pyplot as plt
import numpy as np
from mpl_toolkits.mplot3d.art3d import Poly3DCollection

def read_triangles(filename):
    triangles = []
    with open(filename, 'r') as f:
        # Читаем первую строку и игнорируем комментарии
        line = f.readline().strip()
        while line.startswith('#'):
            line = f.readline().strip()
        
        n = int(line)
        
        if n != 0:
            for _ in range(n):
                points = []
                points_read = 0
                while points_read < 3:
                    line = f.readline().strip()
                    # Пропускаем пустые строки и комментарии
                    if not line or line.startswith('#'):
                        continue
                    
                    # Заменяем запятые на пробелы и разбиваем
                    line = line.replace(',', ' ')
                    coords = list(map(float, line.split()))
                    points.append(coords)
                    points_read += 1
                
                triangles.append(points)
    return triangles

def plot_triangles(triangles, filename):
    fig = plt.figure(figsize=(10, 8))
    ax = fig.add_subplot(111, projection='3d')
    
    colors = plt.cm.tab10(np.linspace(0, 1, len(triangles)))
    
    for i, triangle in enumerate(triangles):
        tri_array = np.array(triangle)
        poly = Poly3DCollection([tri_array], alpha=0.5)
        poly.set_facecolor(colors[i])
        poly.set_edgecolor('black')
        poly.set_linewidth(2)
        ax.add_collection3d(poly)
    
    all_points = np.vstack(triangles)
    min_vals = all_points.min(axis=0) - 1
    max_vals = all_points.max(axis=0) + 1
    
    ax.set_xlim(min_vals[0], max_vals[0])
    ax.set_ylim(min_vals[1], max_vals[1])
    ax.set_zlim(min_vals[2], max_vals[2])
    
    ax.set_xlabel('X')
    ax.set_ylabel('Y')
    ax.set_zlabel('Z')
    ax.set_title(f'3D Triangles from {filename}')
    
    plt.tight_layout()
    plt.savefig('triangles_3d.png', dpi=300, bbox_inches='tight')
    plt.show()

if __name__ == "__main__":
    filename = "/home/matvey/work/Triangles_Rebellion/tests/GEOLib/e2e/dat/test7.dat"
    triangles = read_triangles(filename)
    plot_triangles(triangles, filename)