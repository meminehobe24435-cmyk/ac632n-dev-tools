import fs from "node:fs/promises";
import { Presentation, PresentationFile } from "@oai/artifact-tool";

const OUT = "D:/zhuomian/研发团队工作/研发团队工作日报/研发日报项目总结_20260709-20260713.pptx";
const QA = "D:/23178/JL/ppt_work/daily_report_summary/tmp/qa";

const W = 1280;
const H = 720;
const C = {
  ink: "#111111",
  muted: "#555555",
  quiet: "#8A8F98",
  panel: "#F1F1F1",
  panel2: "#E6E8EB",
  rule: "#B8BCC4",
  accent: "#FF6B35",
  accent2: "#2563EB",
  ok: "#16A34A",
  canvas: "#FFFFFF",
};
const font = "Microsoft YaHei";

async function writeBlob(path, blob) {
  await fs.writeFile(path, new Uint8Array(await blob.arrayBuffer()));
}

function addText(slide, text, x, y, w, h, opt = {}) {
  const shape = slide.shapes.add({
    geometry: "textbox",
    position: { left: x, top: y, width: w, height: h },
    fill: "none",
    line: { style: "solid", fill: "none", width: 0 },
  });
  shape.text = text;
  shape.text.style = {
    fontSize: opt.size ?? 20,
    bold: opt.bold ?? false,
    color: opt.color ?? C.ink,
    typeface: font,
    alignment: opt.align ?? "left",
  };
  return shape;
}

function addBox(slide, x, y, w, h, fill = C.panel, line = C.rule) {
  return slide.shapes.add({
    geometry: "rect",
    position: { left: x, top: y, width: w, height: h },
    fill,
    line: { style: "solid", fill: line, width: 1 },
  });
}

function addRule(slide, x, y, w, color = C.ink) {
  slide.shapes.add({
    geometry: "rect",
    position: { left: x, top: y, width: w, height: 2 },
    fill: color,
    line: { style: "solid", fill: color, width: 0 },
  });
}

function titleSlide(slide, title, subtitle, footer) {
  slide.background.fill = C.canvas;
  addText(slide, "研发进展汇报", 54, 48, 280, 34, { size: 18, bold: true, color: C.muted });
  addRule(slide, 54, 98, 1168, C.ink);
  addText(slide, title, 54, 172, 920, 150, { size: 58, bold: true });
  addText(slide, subtitle, 58, 360, 760, 70, { size: 24, color: C.muted });
  addBox(slide, 880, 172, 286, 286, C.panel, C.panel);
  addText(slide, "4", 915, 205, 96, 76, { size: 64, bold: true, color: C.accent });
  addText(slide, "份日报\n10页汇报\n4条项目线", 915, 300, 210, 118, { size: 24, bold: true });
  addText(slide, footer, 58, 640, 780, 30, { size: 18, color: C.quiet });
}

function addSlideTitle(slide, title, kicker = "研发日报整理") {
  slide.background.fill = C.canvas;
  addText(slide, kicker, 54, 38, 360, 28, { size: 16, bold: true, color: C.muted });
  addText(slide, title, 54, 78, 980, 54, { size: 38, bold: true });
  addRule(slide, 54, 145, 1168, C.rule);
}

function bulletList(slide, items, x, y, w, h, size = 20) {
  const text = items.map((v) => `• ${v}`).join("\n");
  return addText(slide, text, x, y, w, h, { size, color: C.ink });
}

function metric(slide, value, label, x, y, w, color = C.accent) {
  addText(slide, value, x, y, w, 72, { size: 54, bold: true, color });
  addText(slide, label, x, y + 74, w, 46, { size: 18, color: C.muted });
}

function card(slide, title, body, x, y, w, h, accent = C.ink) {
  addBox(slide, x, y, w, h, C.panel, C.panel);
  addText(slide, title, x + 22, y + 18, w - 44, 32, { size: 22, bold: true, color: accent });
  addText(slide, body, x + 22, y + 62, w - 44, h - 78, { size: 17, color: C.ink });
}

const p = Presentation.create({ slideSize: { width: W, height: H } });

// 1
titleSlide(
  p.slides.add(),
  "研发日报项目进展总结",
  "基于 2026/07/09 至 2026/07/13 工作日报整理，覆盖蓝牙、无人机、机械臂喷涂和工具链建设。",
  "尤译庆 | 2026年07月"
);

// 2
{
  const s = p.slides.add();
  addSlideTitle(s, "四天工作围绕三个项目和一条工具链展开");
  metric(s, "3", "项目方向\n蓝牙 / 无人机 / 喷涂", 70, 205, 260);
  metric(s, "4", "日报记录\n7月9日、10日、11日、13日", 370, 205, 320, C.accent2);
  metric(s, "1", "共同目标\n把验证流程沉淀为可复用方法", 760, 205, 360, C.ok);
  addText(s, "本阶段不是单点调试，而是在逐步形成“环境搭建、硬件验证、算法调研、文档沉淀”的研发闭环。", 72, 500, 1060, 78, { size: 24, bold: true });
}

// 3
{
  const s = p.slides.add();
  addSlideTitle(s, "项目进展按时间形成清晰推进节奏");
  const dates = [
    ["07/09", "无人机方案与 Betaflight 环境"],
    ["07/10", "杰理蓝牙环境、喷涂文档、无人机选型"],
    ["07/11", "项目资料与阶段任务整理"],
    ["07/13", "蓝牙改名验证、串口方案、动态改名分析"],
  ];
  dates.forEach((d, i) => {
    const x = 82 + i * 290;
    addBox(s, x, 215, 230, 190, i === 3 ? "#FFF1EA" : C.panel, i === 3 ? C.accent : C.panel);
    addText(s, d[0], x + 18, 238, 120, 48, { size: 32, bold: true, color: i === 3 ? C.accent : C.ink });
    addText(s, d[1], x + 18, 305, 188, 78, { size: 19, bold: true });
  });
  addText(s, "时间线显示：前期以环境和方案搭建为主，后期进入实机验证、接口分析和复用模板沉淀。", 84, 490, 1020, 62, { size: 23, color: C.muted });
}

// 4
{
  const s = p.slides.add();
  addSlideTitle(s, "杰理蓝牙模块已经完成名称修改和扫描验证");
  card(s, "环境与工程", "定位 fw-AC63_BT_SDK、Code::Blocks、AC632N_spp_and_le 工程，建立编译和下载流程。", 70, 190, 340, 160);
  card(s, "名称修改", "将默认蓝牙名称修改为 WTYI_BT_TEST，并同步处理配置区旧名称覆盖问题。", 470, 190, 340, 160, C.accent);
  card(s, "扫描验证", "电脑 BLE 扫描确认广播名称为 WTYI_BT_TEST(BLE)，说明固件运行和广播链路已打通。", 870, 190, 340, 160, C.ok);
  addBox(s, 70, 425, 1140, 86, "#FFFFFF", C.rule);
  addText(s, "关键判断：如果板子停留在 BD19 UBOOT 下载模式，手机和电脑都扫不到 BLE 广播；复位到 JLDebug_V0.1 后才能验证运行状态。", 95, 447, 1080, 42, { size: 22, bold: true });
}

// 5
{
  const s = p.slides.add();
  addSlideTitle(s, "串口打印方案需要区分 Type-C 和 UART 调试口");
  addBox(s, 72, 190, 520, 320, C.panel, C.panel);
  addText(s, "已确认", 102, 220, 180, 32, { size: 24, bold: true, color: C.ok });
  bulletList(s, [
    "当前工程 UART0 TX 为 PA00",
    "默认波特率为 1000000",
    "已编写 Python 串口接收脚本",
    "日志可保存到 D 盘目录",
  ], 102, 276, 430, 180, 20);
  addBox(s, 665, 190, 520, 320, "#FFF1EA", C.accent);
  addText(s, "需要注意", 695, 220, 200, 32, { size: 24, bold: true, color: C.accent });
  bulletList(s, [
    "P1 是电池接口，不是串口",
    "PA00 未直接引到 P1",
    "直接焊芯片脚风险较高",
    "可评估 USB CDC 走 Type-C 打印",
  ], 695, 276, 430, 180, 20);
}

// 6
{
  const s = p.slides.add();
  addSlideTitle(s, "动态蓝牙名称修改已有技术路径但仍需实机验证");
  card(s, "已发现接口", "bt_set_local_name() 可以修改内存中的本地名，bt_get_local_name() 会被 BLE 广播名生成逻辑读取。", 72, 190, 355, 180);
  card(s, "优先方案", "运行中切换名称后，重新生成 adv/rsp 广播包，再调用广播配置接口刷新扫描名称。", 462, 190, 355, 180, C.accent2);
  card(s, "兜底方案", "如果直接刷新广播不生效，则关闭 BLE 模块后重新 bt_ble_exit() + bt_ble_init()。", 852, 190, 355, 180, C.accent);
  addText(s, "对外口径：SDK 有基础接口，但尚未确认有“运行时一键改名并自动刷新广播”的封装接口；下一步做 10 秒切名 demo 验证。", 92, 465, 1040, 64, { size: 23, bold: true });
}

// 7
{
  const s = p.slides.add();
  addSlideTitle(s, "无人机方向从方案论证推进到飞控工具链准备");
  addBox(s, 72, 185, 356, 260, C.panel, C.panel);
  addText(s, "方案方向", 98, 212, 180, 32, { size: 24, bold: true });
  bulletList(s, ["天花板吸附攀爬", "真空吸附闭环", "轮式攀爬底盘", "PX4/ArduPilot + ROS 2"], 98, 266, 270, 138, 19);
  addBox(s, 462, 185, 356, 260, C.panel, C.panel);
  addText(s, "工具链", 488, 212, 180, 32, { size: 24, bold: true });
  bulletList(s, ["MSYS2 安装到 D 盘", "Betaflight 编译依赖", "git/make/python/gcc 验证", "桌面快捷入口"], 488, 266, 270, 138, 19);
  addBox(s, 852, 185, 356, 260, C.panel, C.panel);
  addText(s, "下一步", 878, 212, 180, 32, { size: 24, bold: true, color: C.accent });
  bulletList(s, ["先确定开发板型号", "同步推进飞控算法", "梳理小车驱动算法", "为结构预留安装空间"], 878, 266, 270, 138, 19);
}

// 8
{
  const s = p.slides.add();
  addSlideTitle(s, "机械臂喷涂方向聚焦末端位置控制和工艺一致性");
  addText(s, "当前完成了机械臂喷漆位置控制文档了解，重点围绕轨迹、姿态、覆盖和喷涂稳定性建立技术关注点。", 74, 172, 1010, 56, { size: 22, color: C.muted });
  const rows = [
    ["轨迹规划", "喷涂路径、往复轨迹、边角补喷、搭接率"],
    ["末端控制", "喷枪到工件距离、法向角度、速度稳定性"],
    ["质量验证", "覆盖范围、厚度均匀性、漏喷和堆漆控制"],
  ];
  rows.forEach((r, i) => {
    const y = 265 + i * 105;
    addBox(s, 82, y, 240, 68, i === 1 ? "#FFF1EA" : C.panel, i === 1 ? C.accent : C.panel);
    addText(s, r[0], 108, y + 16, 180, 32, { size: 24, bold: true, color: i === 1 ? C.accent : C.ink });
    addText(s, r[1], 360, y + 17, 720, 32, { size: 22 });
  });
}

// 9
{
  const s = p.slides.add();
  addSlideTitle(s, "工具和知识沉淀让后续项目更容易复用");
  card(s, "Python 工具", "BLE 扫描和串口接收工具已就绪，支持后续快速验证广播和运行日志。", 80, 205, 330, 190, C.accent2);
  card(s, "项目 Skills", "新增杰理蓝牙、无人机飞控、机器人喷涂三个项目 skill，便于复用固定流程。", 475, 205, 330, 190, C.ok);
  card(s, "日报体系", "基于每日记录持续形成可追溯的工作闭环、问题卡点和次日计划。", 870, 205, 330, 190, C.accent);
  addText(s, "沉淀方向从“今天解决一个问题”升级为“后续同类问题可以按模板推进”。", 100, 485, 900, 46, { size: 25, bold: true });
}

// 10
{
  const s = p.slides.add();
  addSlideTitle(s, "下一阶段优先完成三个可验证闭环");
  const items = [
    ["P0", "蓝牙动态改名 demo", "实现开机前 10 秒名称一、后 10 秒名称二，并用手机和电脑同时验证。"],
    ["P1", "Type-C 日志方案", "评估 USB CDC 虚拟串口，降低焊接 PA00 测试点的调试风险。"],
    ["P2", "项目规划清单", "继续完善无人机开发板选型和喷涂位置控制需求清单。"],
  ];
  items.forEach((it, i) => {
    const y = 190 + i * 125;
    addText(s, it[0], 88, y, 90, 52, { size: 38, bold: true, color: i === 0 ? C.accent : C.ink });
    addBox(s, 190, y - 2, 2, 76, C.rule, C.rule);
    addText(s, it[1], 225, y, 410, 34, { size: 25, bold: true });
    addText(s, it[2], 225, y + 42, 850, 38, { size: 20, color: C.muted });
  });
  addText(s, "结论：当前阶段的重点是把“能改、能扫、能记录”推进到“能动态切换、能稳定调试、能持续复用”。", 86, 600, 1050, 54, { size: 23, bold: true });
}

await fs.mkdir(QA, { recursive: true });
for (const [index, slide] of p.slides.items.entries()) {
  const stem = `slide-${String(index + 1).padStart(2, "0")}`;
  await writeBlob(`${QA}/${stem}.png`, await p.export({ slide, format: "png", scale: 1 }));
  await fs.writeFile(`${QA}/${stem}.layout.json`, await (await slide.export({ format: "layout" })).text());
}
await writeBlob(`${QA}/deck-montage.webp`, await p.export({ format: "webp", montage: true, scale: 1 }));

const pptx = await PresentationFile.exportPptx(p);
await pptx.save(OUT);
console.log(OUT);
