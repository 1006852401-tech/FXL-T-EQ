# Seesaw Tilt EQ (AU & VST3 Plugin)
跷跷板模式单旋钮均衡器插件 (AU / VST3 / Standalone)

## 插件特性
- **单旋钮对称设计 (Single Knob)**：刻度居中（0 dB / 12点钟方向为平直状态，无音染）。
- **右旋 (Clockwise)**：高频平滑提升，低频对称衰减（音色明亮清澈）。
- **左旋 (Counter-Clockwise)**：低频平滑提升，高频对称衰减（音色温暖浑厚）。
- **中心枢轴频率 (Pivot)**：标准音频工程黄金点 650 Hz。
- **无爆音平滑滤波**：包含线性参数平滑与高质量双二阶滤波器 (Biquad Filter)。

---

## 编译方法（生成 AU 与 VST3 格式插件）

本项目基于标准的 **JUCE 7 / 8 + CMake** 架构，可同时编译出：
- **Apple Audio Unit (AU / .component)** 格式（支持 Logic Pro, GarageBand, Ableton Live macOS）
- **Steinberg VST3 (.vst3)** 格式（支持 Cubase, FL Studio, Ableton Live, Reaper, Studio One 等）
- **Standalone** 独立桌面应用

### 方案 A：使用 GitHub Actions 自动云端编译（无需本地安装任何编译器！）
本项目已预置 `.github/workflows/build-plugins.yml`：
1. 在 GitHub 上新建一个免费代码仓库（例如 `my-seesaw-eq`）。
2. 将本 ZIP 压缩包解压后的所有文件上传/推送至该仓库。
3. 进入仓库页面的 **Actions** 标签，GitHub 会自动启动 **macOS 最新系统**（自动编译 AU 与 VST3）与 **Windows 系统**（自动编译 VST3）。
4. 大约 2~3 分钟后编译完成，直接在 Actions 运行记录下方的 **Artifacts** 区域即可一键下载已编译打包好的插件二进制包：
   - `SeesawTiltEQ_macOS_AU_VST3.zip` (包含原生 .component 和 .vst3)
   - `SeesawTiltEQ_Windows_VST3.zip` (包含原生 .vst3)

### 方案 B：本地终端一键脚本编译 (macOS / Windows)
本项目已预置自动下载 JUCE 8 与编译脚本：
- **macOS 用户**：
  ```bash
  chmod +x build_mac.sh
  ./build_mac.sh
  ```
  编译完成后会自动安装至系统插件目录：
  - AU: `~/Library/Audio/Plug-Ins/Components/Seesaw Tilt EQ.component`
  - VST3: `~/Library/Audio/Plug-Ins/VST3/Seesaw Tilt EQ.vst3`

- **Windows 用户**：
  双击运行 `build_win.bat` 即可自动完成构建。

### 方案 C：使用 Faust Web IDE 导出
随附的 `Faust/tilt_eq.dsp` 包含纯 DSP 描述。
注意：由于 Faust Web IDE 远程服务器为 Linux 虚拟机，无法直接为 macOS 运行 Xcode 编译器，因此若在 Faust IDE 导出时提示架构错误，请在导出时选择：
- **Platform: source** -> **Architecture: juce** 或 **cplusplus**
- 或使用方案 A 的 GitHub Actions 获取原生编译好的二进制文件。
