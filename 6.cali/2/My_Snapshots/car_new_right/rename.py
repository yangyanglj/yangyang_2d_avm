#@Author  :JinXueXiang
#@Time    :2019/8/15 10:31
#@FileName: 批量图片文件重命名.py
#@Software: PyCharm

# -*- coding:utf8 -*-

import os

class BatchRename():
    '''
    批量重命名文件夹中的图片文件

    '''
    def __init__(self):
        self.path = '/home/yy/yangyang/1.ransac_opencv/6.cali/2/My_Snapshots/car_new_right'  #表示需要命名处理的文件夹

    def rename(self):
        filelist = os.listdir(self.path)   #获取文件路径
        total_num = len(filelist)  #获取文件长度（个数）
        i = 1  #表示文件的命名是从1开始的
        for item in filelist:
            if item.endswith('.jpg'):  #初始的图片的格式为jpg格式的（或者源文件是png格式及其他格式，后面的转换格式就可以调整为自己需要的格式即可）
                src = os.path.join(os.path.abspath(self.path), item)
                dst = os.path.join(os.path.abspath(self.path), 'img' + format(str(i), '0>1s') + '.jpg')#处理后的格式也为jpg格式的，当然这里可以改成png格式
                # 这种情况下的命名格式为xn000.jpg形式，可以自主定义想要的格式
                try:
                    os.rename(src, dst)
                    print ('converting %s to %s ...' % (src, dst))
                    i = i + 1
                except:
                    continue
        print ('total %d to rename & converted %d jpgs' % (total_num, i))

if __name__ == '__main__':
    demo = BatchRename()
    demo.rename()
