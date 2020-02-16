MediaInputStream \ MediaDecoder \ MediaInputStream 要抽个接口出来

对Player 要有个管理器 工厂类

对外的预加载接口 对应 MediaInputStream的接口

异常抛出

对player的状态进行封装

TODO 继承ConcurrentQueue的子类 对于destory_node 能不能抽象到基类，这样就直接可以用泛型了 主要ffmpeg的对象释放 TransformNodeCachePool

FUTURE 替换成  std:thread