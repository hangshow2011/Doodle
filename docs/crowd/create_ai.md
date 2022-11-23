# ai创建
## 创建类
- 创建三个类
  ![](主界面.jpg)
  - 创建角色蓝图、动画蓝图、动画混合空间
    - 创建角色蓝图需要继承**DoodleAiCrowd**类 
    - 创建动画蓝图需要继承**DoodleAnimInstance**类
    - 创建动画蓝图时目标骨骼需要是指定ai的动画骨骼
    - 创建动画混合空间时目标骨骼需要是指定ai的动画骨骼
  - 创建动画蓝图和动画混合空间(添加/导入——>创建高级资产——>动画)  
- 创建导航网格边界体积界面  
![](设置导航网格边界体积界面.jpg)
  - 移动范围： 在放置Actor里 找到导航网格体边界体积(NavMeshBoundsVolume)  
  **注意**：框选范围需要涵盖你需要的范围以及你放置物体的体积范围 

## 角色蓝图

- 角色蓝图类默认值
  ![](角色蓝图类默认值.jpg)
  - 细节——>Doodle——>勾选Move To
  - 细节——>pawn——>AI控制器类——>DoodleAIController    
- 角色蓝图网格体  
  ![](角色蓝图网格体.jpg)  
  - 细节————>动画————>设置动画模式————>使用动画蓝图  
  - 细节————>动画————>设置动画类————>我们刚刚创建的动画蓝图
  - 细节————>网格体————>选择网格体  
- 角色蓝图_角色移动
  ![](角色蓝图_角色移动.jpg)
  **建议勾选**
  - 细节————>角色移动————>调整Z轴旋转方向为60度
  - 细节————>角色移动————>勾选将旋转朝向运动

**注意**：记得编译保存！！
## 动画混合空间
![](动画混合空间.jpg) 
- 设置水平坐标轴   
  - Axis setting————>水平坐标————>设置名称   

  - Axis setting————>水平坐标————>最大最小轴值   
    轴的最大值应该和跑步动画（可以认为是动画混合空间中的最快的那个动画）的最大速度（动画在实际中跑步的速度）相同


- 预览场景设置(可以不用设置，只是用于预览）————>网格体————>选择你需要的网格体 
- 资源浏览器————>你需要的动作————>点击拖拽到视口界面
  - 白色的点左右移动代表动作开始、结束
  - 绿色的点左右移动可以预览人物动作效果。

## 动画蓝图
- 类设置
![](动画蓝图类设置.jpg)
  - 类设置————>细节————>类选项————>父类————>导入Doodle Anim Instance  
  - 将混合空间播放器拖到蓝图中————>创建读取Velocity Attr的值的对象————>将三个对象用线连接起来
 
 




