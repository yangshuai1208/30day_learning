# Day 02｜C结构体、Codex Diff审查与STM32 STOP工程化

**日期：** 2026年11月5日  
**训练主题：** C语言结构体、AI多文件修改、状态机安全保护、HAL Mock测试

## 1. C语言知识

### 结构体与内存对齐
- 结构体成员按各自的对齐要求排列，必要时插入填充字节。
- `sizeof(struct)`可能大于成员大小之和。
- 内存对齐与硬件访问要求、ABI和访问效率有关。

### 结构体传参
- 传值：函数获得结构体副本，修改副本不影响原对象。
- 传指针：通过`->`访问结构体，可修改原对象。
- const结构体指针：禁止通过该指针修改结构体内容。

### 位操作
- `1U << channel`：生成指定通道掩码。
- `|=`：设置指定标志位，同时保留其他标志位。
- 更新前必须检查指针、下标和数据范围。

### 今日代码
`code/day02/servo_state.c`

实现舵机角度更新和位掩码维护，已完成代码修正及GCC编译。

## 2. Codex AI工程技能

学习了以下工作流：

- 读取真实项目上下文，确认已有功能。
- 使用Git Diff检查AI修改范围。
- 通过只读审查保护既有未提交修改。
- 限制AI修改指定文件，避免大规模重构。
- 使用AI生成HAL Mock及PC端测试。

核心原则：AI修改之后必须由开发者审查、编译和验证。

## 3. STM32 STOP方案A

涉及生产文件：

- `Core/Src/hand_servo.c`
- `Core/Src/main.c`

主要修改：

- STOP取消当前运动轨迹。
- 目标角度保持为软件记录的最近成功下发角度。
- 正常状态下STOP进入IDLE。
- FAULT状态下STOP保留FAULT。
- 不新增STOP期间的I²C写入，不主动关闭PWM。
- 原活动命令被抢占后返回PREEMPTED。
- STOP成功后直接返回OK。

注意：STOP成功不代表机械手物理上立即停止。

## 4. FAULT故障保护

在`hand_servo_start_action()`入口增加故障状态检查。

- FAULT下拒绝OPEN、GRAB、RELEASE。
- FAULT下允许STOP取消软件轨迹。
- 被拒绝命令返回HAL_ERROR。
- 主程序使用现有ERROR ACK分支处理失败命令。

遗留风险：部分公开的阻塞舵机控制接口仍可能绕过这一入口检查。

## 5. ACK与seq可靠性

原问题：ACK只有4项环形缓存，活动命令的记录可能被淘汰。

修复思路：

- 查询ACK缓存前优先检查active_seq和active_action。
- 相同seq与相同action：回复IN_PROGRESS。
- 相同seq与不同action：回复SEQ_CONFLICT。
- 不同seq：继续原有处理流程。
- STOP使用新的seq抢占普通动作。

限制：缓存淘汰之后，已经完成的历史命令不能保证永久去重。

## 6. PC端测试与编译结果

**Keil：**
- STM32F407工程编译通过。
- 0 Errors，0 Warnings。

**GCC协议解析：**
- 使用真实`hand_protocol.c`。
- 测试输出：`hand_protocol passed`、`TEST_PASS`。

**HAL Mock状态机测试：**
- 使用真实`hand_servo.c`和`pca9685.c`。
- 模拟HAL Tick、I²C写入及故障返回。
- 测试命令退出码为0。
- 测试涉及STOP、FAULT和普通动作准入。

**尚未验证：**
- ACK/seq主循环分发的独立PC端单元测试。
- 真实硬件的STOP时序及物理行为。

按当前训练安排，实机验证暂停。

## 7. 今日错题与面试总结

- HAL Mock用于模拟硬件依赖，不能完全代替实机测试。
- STOP负责取消运动轨迹，FAULT用于表示系统故障，两者不能混淆。
- active_seq优先于缓存检查，是为了保护正在执行的命令身份。
- 编译成功仅验证构建过程，单元测试用于验证软件行为。

## 8. 后续待办

- 补充ACK/seq真实生产分发逻辑的PC端测试。
- 评估FAULT状态下直接舵机控制接口的保护策略。
- 检查初始化阶段current_angles更新顺序。
- 评估同步UART/I²C对STOP响应延迟的影响。
- 后续按原定Day 3计划进入舵机节拍与平滑控制。

## 9. 今日结论

Day 2完成了STOP方案A的最小工程修改、FAULT保护、活动seq保护、Keil编译及部分PC端单元测试。

**状态：软件开发与已执行测试取得阶段性成果，未进行实机验证。**