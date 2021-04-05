1. 绑定  
   1. register交给用户（每个renderpass使用一个root signature，各个renderpass之间namespace不冲突）
   2. 每个graphicsPass register用不同的space
2. pipeline
   1. 抽象为pass，并且根据resource的依赖关系来确定是否需要barrier（没有barrier就可以并行执行）
   2. 每个graphicsPass 遍历自己的资源，如果当前状态不为目标状态，则转换
   3. uav barrier需要用户自己确定
3. 资源
   1. 不保存descriptor
   2. 储存当前状态

4. rendertarget
   1. 不是资源，是rtv的封装
   2. 可以通过一系列texture来初始化（attachment），以构建地址连续的RTV

5. ray tracing




Context保持API相关的资源（commandList device）
Factory用于创建资源
Engine用于逻辑上操作

Factory包含Context  Engine包含Context  Resource包含Factory  Resource包含Factory


通过Factory创建资源  来创建api相关资源
graphics pass 与Api无关
所有**绑定**操作需要在资源里面实现    graphics pass只需要调用接口就可以实现

通过ShaderObject创建shader resource binding
shader resource binding和render target提供bind接口 