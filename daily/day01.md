
## STM32F407机械手STOP方案设计

### 1. 现有架构

- 已存在基于HAL_GetTick的非阻塞动作状态机。
- 已存在UART DMA、环形缓冲与协议解析逻辑。
- 已存在ACK、seq及短期重复命令处理。
- STOP当前会取消原动作并转向90°目标角度。
- 同步I2C和UART操作可能影响STOP响应延迟。

以上结论来自Codex只读源码审查。

### 2. Day 1设计决策

选择方案A：STOP取消轨迹，保持最后成功下发的目标角度。

计划实现：

1. STOP取消当前正在执行的动作。
2. target_angles更新为软件记录的current_angles。
3. 将动作状态切换为HAND_MOTION_IDLE。
4. 不新增I2C写入，不主动关闭PWM。
5. 原动作ACK记录为PREEMPTED。
6. STOP接管成功后直接返回OK。
7. 保留现有seq去重逻辑。

### 3. 需要修改的函数

- hand_servo_start_action()
- main.c中的STOP处理分支

尽量不修改其他模块。

### 4. 后续验证

- 有活动动作时发送STOP
- 空闲状态发送STOP
- 连续STOP使用不同seq
- 重复发送相同seq
- 相同seq对应不同命令
- STOP后无额外舵机目标角度更新
- STOP失败时不错误返回OK
- 初始化失败时current_angles的有效性

### 5. 当前状态

方案设计：完成
源码修改：未开始
编译验证：未开始
独立测试：未开始
实机验证：未开始

注：STOP的OK仅表示软件轨迹取消成功，
不保证机械手已经物理静止。
