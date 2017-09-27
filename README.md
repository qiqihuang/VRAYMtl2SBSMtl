# VRAYMtl2SBSMtl
### Convert VRay material to substance material for import to Unity3d
### （转换VRay材质到Substance材质 以便于导入到Unity3d）

# 需求
### Unity3d不支持VRay材质系统，而VRay又被广泛使用
### Unity3d使用PBR材质系统，而substance是典型代表

# 解决方案
### 通过一些曲线拟合方法（简单的使用了Execl提供的曲线拟合功能） 就 VRay参数转换为substance参数，具体的就是转换到PBR材质需要的 Albedo、Metallic、Normap Map和Height Map

# 程序结构
### VRayMtl2SBS.ms(3ds Max 2014) 基于[VRayMtlConverter v2.5](http://www.scriptspot.com/3ds-max/scripts/vraymtlconverter-v2-5)实现
具体就是实现了VRay材质到Substance材质所必须参数的生成，结果存到一个临时JSON文件中，如：
``` JSON
{
	"name": "f407b42f-86b7-48b7-8ff0-5c46c760dcff",
	"basecolor": {
		"bitmap": "C:/Users/QI/Desktop/9247/Tikva_001.jpg",
		"color": "0.430326 0.430326 0.430326 1.0"
	},
	"normal": {
		"bitmap": "C:/Users/QI/Desktop/9247/ed307d03-c373-4fdf-b2c5-2d280f6c43b7.jpg",
		"intensity": "0.05"
	},
	"roughness": {
		"color": "0.443961 0.443961 0.443961 1.0"
	},
	"metallic": {
		"color": "0.0224084 0.0224084 0.0224084 1.0"
	}
}
```

### template.sbs文件为Substance Designer模板（如需运行程序，请安装[Substance Designer](https://www.allegorithmic.com/)程序），C++代码的工作就是将JSON内容填充到Substance Designer模板中，得到sbs文件。

### sbs文件为设计文件（类似于max），而Unity3d能识别的材质是sbsar（类似于fbx），从sbs到sbsar，需要使用Substance Designer提供的烘焙功能。C++代码中通过命令行调用Substance Designer的sbscooker.exe程序完成材质烘焙。