//主界面的实现文件，包含中央窗口、范围类、芯片状态项类和底部日志栏的实现
//

#include "CentralWindow.hpp"
#include "DlgMenuFile.hpp"
#include <QDesktopServices>
#include <QUrl>
#include <QFileInfo>
#include <QMessageBox>
#include <QDateTime>
#include <QString>
#include <QFileDialog>
#include <QTextStream>
#include <QFile>

/************************* CWD命名空间：中央窗口相关类的实现 *************************/
namespace CWD {

	/************************* Range类：表示一个范围区间的实现 *************************/
	Range::Range()
		:Left(0), Right(0)                                                                                                                    // 构造函数，初始化左右端点为0
	{
	}

	Range::Range(long long left, long long right)
		:Left(left), Right(right)                                                                                                             // 构造函数，初始化左右端点为指定值
	{
	}

	Range::Range(const Range& range)
		:Left(range.Left), Right(range.Right)                                                                                                 // 拷贝构造函数
	{
	}

	Range::Range(const TCOND::Range& range)
		:Left(range.GetLeft()), Right(range.GetRight())                                                                                       // 从TCOND::Range构造
	{
	}

	Range::~Range()                                                                                                                           // 析构函数
	{
	}

	void Range::operator=(const Range& range)
	{
		Left = range.Left;                                                                                                                    // 赋值左端点
		Right = range.Right;                                                                                                                  // 赋值右端点
	}

	void Range::SetRange(long long left, long long right)
	{
		Left = left;                                                                                                                          // 设置左端点
		Right = right;                                                                                                                        // 设置右端点
	}

	long long Range::GetLeft() const
	{
		return Left;                                                                                                                          // 获取左端点
	}

	long long Range::GetRight() const
	{
		return Right;                                                                                                                         // 获取右端点
	}

	/************************* Item类：表示单个芯片状态项的实现 *************************/

	Item::Item() :IfValid(false), OnLineStat(INITIAL_OFF_LINE), TestRes(), SinadRang() {}                                                     // 默认构造函数，初始化为离线无效状态

	Item::Item(bool b, Range tr, Range sr) :IfValid(b), OnLineStat(INITIAL_OFF_LINE), TestRes(tr), SinadRang(sr) {}                           // 带参构造函数，初始化所有成员变量

	Item::Item(const Item& cp) :IfValid(cp.IfValid), OnLineStat(cp.OnLineStat), TestRes(cp.TestRes), SinadRang(cp.SinadRang) {}               // 拷贝构造函数，复制所有成员

	Item::~Item() {}                                                                                                                          // 析构函数

	void Item::operator=(const Item& as)
	{
		IfValid = as.IfValid;                                                                                                                 // 赋值有效标志
		OnLineStat = as.OnLineStat;                                                                                                           // 赋值在线状态
		TestRes = as.TestRes;                                                                                                                 // 赋值测试结果范围
		SinadRang = as.SinadRang;                                                                                                             // 赋值SINAD范围
	}

	void Item::RestOnLineStat()
	{
		OnLineStat = INITIAL_OFF_LINE;                                                                                                        // 重置在线状态为初始离线状态
	}

	/************************* Item绘制方法：负责在界面上绘制芯片状态 *************************/
	void Item::Paint(QPainter* painter, const QStyleOptionViewItem& option, int row)
	{
                                                                                                                                              // WRITE_CENTRAL_WIDGET_DBG("Item::Paint(), Enter\n");
		if (row > 0)                                                                                                                          // 只绘制数据行，不绘制表头
		{

			painter->save();                                                                                                                  // 保存画笔状态

			int Left = option.rect.left();                                                                                                    // 获取绘制区域左边界
			int Top = option.rect.top();                                                                                                      // 获取绘制区域上边界
			int Width = option.rect.width();                                                                                                  // 获取绘制区域宽度
			int Height = option.rect.height();                                                                                                // 获取绘制区域高度
			int CellWidth = Width / 8;                                                                                                        // 将宽度分成8列

                                                                                                                                              // QStyleOptionProgressBar ProgressBarOption;
                                                                                                                                              // QPalette palette = ProgressBarOption.palette;

			QRect Rc[3] = {                                                                                                                   // 定义三个绘制矩形区域
				QRect(Left, Top, CellWidth - 3, Height - 2),                                                                                  // 芯片编号区域
				QRect(Left + CellWidth, Top, CellWidth * 5 - 3, Height - 2),                                                                  // 进度条区域
				QRect(Left + CellWidth * 6, Top, CellWidth * 2 - 3, Height - 2)                                                               // SINAD范围显示区域
			};
                                                                                                                                              // painter->setPen(QPen(Qt::black));
			QFont font("Microsoft YaHei");                                                                                                    // 设置微软雅黑字体
			painter->setFont(font);                                                                                                           // 应用字体设置

			if (IfValid)                                                                                                                      // 如果芯片有效（在线）
			{
                                                                                                                                              // WRITE_CENTRAL_WIDGET_DBG("Chip:%d, Online, Satisfied cnt = %d, Completed cnt = %d\n",
                                                                                                                                              //     row - 1, TestRes.GetLeft(), TestRes.GetRight());
				switch (OnLineStat)                                                                                                           // 根据在线状态进行不同的绘制
				{
				case INITIAL_OFF_LINE:                                                                                                        // 初始离线状态转换为正常在线
				{
					OnLineStat = NORMAL_ON_LINE;                                                                                              // 状态转换为正常在线
					painter->fillRect(Rc[0], QColor(Qt::green));                                                                              // 状态指示区域填充绿色
					if (TestRes.GetLeft() == TestRes.GetRight() && TestRes.GetRight() != 0)                                                   // 测试完全通过
					{
						painter->setPen(QPen(QColor(Qt::green), 2));                                                                          // 设置绿色边框
						painter->setBrush(QColor(Qt::white));                                                                                 // 设置白色填充
						painter->drawRoundedRect(Rc[1].adjusted(1, 1, -1, -1), 2, 2);                                                         // 绘制圆角矩形

                                                                                                                                              // 绘制进度条
						QRect progressRect = Rc[1].adjusted(2, 2, -2, -2);                                                                    // 进度条矩形
						painter->fillRect(progressRect, QColor(Qt::green));                                                                   // 填充绿色表示成功
					}

					else if(TestRes.GetLeft() == TestRes.GetRight())
					{
						painter->setPen(QPen(QColor(Qt::cyan), 2));                                                                          // 设置蓝色边框
						painter->setBrush(QColor(Qt::white));                                                                                 // 设置白色填充
						painter->drawRoundedRect(Rc[1].adjusted(1, 1, -1, -1), 2, 2);                                                         // 绘制圆角矩形

						// 绘制进度条
						QRect progressRect = Rc[1].adjusted(2, 2, -2, -2);                                                                    // 进度条矩形
						painter->fillRect(progressRect, QColor(Qt::cyan));                                                                   // 填充蓝色表示等待中
					}

					else if (TestRes.GetRight() == 0)                                                                                         // 测试总数为0
					{
                                                                                                                                              // 绘制背景边框
						painter->setPen(QPen(QColor(Qt::lightGray), 2));                                                                      // 设置浅灰色边框
						painter->setBrush(QColor(Qt::lightGray));                                                                             // 设置浅灰色填充
						painter->drawRoundedRect(Rc[1].adjusted(1, 1, -1, -1), 2, 2);

                                                                                                                                               // 绘制进度条
						QRect progressRect = Rc[1].adjusted(2, 2, -2, -2);                                                                   // 进度条矩形
						painter->fillRect(progressRect, QColor(Qt::red));                                                                   // 塕充红色表示异常
					}
					else                                                                                                                      // 测试未完全通过
					{
                                                                                                                                              //绘制背景边框
						painter->setPen(QPen(QColor(Qt::red), 2));                                                                            // 设置红色边框
						painter->setBrush(QColor(Qt::white));                                                                                 // 设置白色填充
						painter->drawRoundedRect(Rc[1].adjusted(1, 1, -1, -1), 2, 2);

                                                                                                                                              // 绘制进度条
						QRect progressRect = Rc[1].adjusted(2, 2, -2, -2);
						painter->fillRect(progressRect, QColor(Qt::red));                                                                     // 填充红色表示失败
					}

					painter->setPen(Qt::black);                                                                                               // 设置黑色文字
					painter->drawText(Rc[1], Qt::AlignCenter, QString("%1\%(%2/%3)").                                                         // 绘制测试进度文本
						arg((TestRes.GetLeft() * 100) / (TestRes.GetRight() + ((TestRes.GetRight() == 0) ? 1 : 0)), 3, 10).
						arg(TestRes.GetLeft(), 3, 10).arg(TestRes.GetRight(), 3, 10));
				}
				break;

				case NORMAL_OFF_LINE:                                                                                                         // 正常离线状态转换为异常在线
				{
					OnLineStat = ABNORMAL_ON_LINE;                                                                                            // 状态转换为异常在线
					painter->fillRect(Rc[0], QColor(Qt::yellow));                                                                             // 状态指示区域填充黄色
                                                                                                                                              // painter->fillRect(Rc[0], QColor(Qt::green));
                                                                                                                                              // palette.setColor(QPalette::ColorRole::Highlight, QColor(Qt::red));
                                                                                                                                              // 绘制背景边框
					painter->setPen(QPen(QColor(Qt::red), 2));                                                                                // 设置红色边框
					painter->setBrush(QColor(Qt::white));                                                                                     // 设置白色填充
					painter->drawRoundedRect(Rc[1].adjusted(1, 1, -1, -1), 2, 2);

                                                                                                                                              //绘制进度条
					QRect progressRect = Rc[1].adjusted(2, 2, -2, -2);
                                                                                                                                              // progressRect.setWidth(progressRect.width() * TestRes.GetLeft() / TestRes.GetRight());
					painter->fillRect(progressRect, QColor(Qt::red));                                                                         // 填充红色表示异常
					painter->setPen(Qt::black);                                                                                               // 设置黑色文字
					painter->drawText(Rc[1], Qt::AlignCenter, QString("%1\%(%2/%3)").                                                         // 绘制测试进度文本
						arg((TestRes.GetLeft() * 100) / (TestRes.GetRight() + ((TestRes.GetRight() == 0) ? 1 : 0)), 3, 10).
						arg(TestRes.GetLeft(), 3, 10).arg(TestRes.GetRight(), 3, 10));
				}

				case NORMAL_ON_LINE:                                                                                                          // 正常在线状态
				{
					painter->fillRect(Rc[0], QColor(Qt::green));                                                                              // 状态指示区域填充绿色
					if (TestRes.GetLeft() == TestRes.GetRight())                                                                              // 测试完全通过
					{
                                                                                                                                              // painter->fillRect(Rc[0], QColor(Qt::green));
                                                                                                                                              // palette.setColor(QPalette::ColorRole::Highlight, QColor(Qt::red));
                                                                                                                                              // 绘制背景边框
						painter->setPen(QPen(QColor(Qt::green), 2));                                                                          // 设置绿色边框
						painter->setBrush(QColor(Qt::white));                                                                                 // 设置白色填充
						painter->drawRoundedRect(Rc[1].adjusted(1, 1, -1, -1), 2, 2);

                                                                                                                                              //绘制进度条
						QRect progressRect = Rc[1].adjusted(2, 2, -2, -2);
						painter->fillRect(progressRect, QColor(Qt::green));                                                                   // 填充绿色表示成功
					}
					else                                                                                                                      // 测试未完全通过
					{
						painter->setPen(QPen(QColor(Qt::red), 2));                                                                            // 设置红色边框
						painter->setBrush(QColor(Qt::white));                                                                                 // 设置白色填充
						painter->drawRoundedRect(Rc[1].adjusted(1, 1, -1, -1), 2, 2);

                                                                                                                                              //绘制进度条
						QRect progressRect = Rc[1].adjusted(2, 2, -2, -2);
                                                                                                                                              // progressRect.setWidth(progressRect.width() * TestRes.GetLeft() / TestRes.GetRight());
						painter->fillRect(progressRect, QColor(Qt::red));                                                                     // 填充红色表示失败
					}
					painter->setPen(Qt::black);                                                                                               // 设置黑色文字
					painter->drawText(Rc[1], Qt::AlignCenter, QString("%1\%(%2/%3)").                                                         // 绘制测试进度文本
						arg((TestRes.GetLeft() * 100) / (TestRes.GetRight() + ((TestRes.GetRight() == 0) ? 1 : 0)), 3, 10).
						arg(TestRes.GetLeft(), 3, 10).arg(TestRes.GetRight(), 3, 10));
				}
				break;
				case ABNORMAL_OFF_LINE:                                                                                                       // 异常离线状态转换为异常在线
					OnLineStat = ABNORMAL_ON_LINE;                                                                                            // 状态转换
				case ABNORMAL_ON_LINE:                                                                                                        // 异常在线状态
				{
					painter->fillRect(Rc[0], QColor(Qt::yellow));                                                                             // 状态指示区域填充黄色
                                                                                                                                              // 绘制背景边框
					painter->setPen(QPen(QColor(Qt::red), 2));                                                                                // 设置红色边框
					painter->setBrush(QColor(Qt::white));                                                                                     // 设置白色填充
					painter->drawRoundedRect(Rc[1].adjusted(1, 1, -1, -1), 2, 2);

                                                                                                                                              //绘制进度条
					QRect progressRect = Rc[1].adjusted(2, 2, -2, -2);
                                                                                                                                              // progressRect.setWidth(progressRect.width() * TestRes.GetLeft() / TestRes.GetRight());
					painter->fillRect(progressRect, QColor(Qt::red));                                                                         // 填充红色表示异常
					painter->setPen(Qt::black);                                                                                               // 设置黑色文字
					painter->drawText(Rc[1], Qt::AlignCenter, QString("%1\%(%2/%3)").                                                         // 绘制测试进度文本
						arg((TestRes.GetLeft() * 100) / (TestRes.GetRight() + ((TestRes.GetRight() == 0) ? 1 : 0)), 3, 10).
						arg(TestRes.GetLeft(), 3, 10).arg(TestRes.GetRight(), 3, 10));
				}
				default:                                                                                                                      // 默认情况
					break;
				}

			}
			else                                                                                                                              // 如果芯片无效（离线）
			{
				switch (OnLineStat)                                                                                                           // 根据在线状态进行不同的绘制
				{
				case INITIAL_OFF_LINE:                                                                                                        // 初始离线状态转换为正常离线
					OnLineStat = NORMAL_OFF_LINE;                                                                                             // 状态转换
				case NORMAL_OFF_LINE:                                                                                                         // 正常离线状态
				{
					painter->fillRect(Rc[0], QColor(Qt::gray));                                                                               // 状态指示区域填充灰色
                                                                                                                                              //绘制背景边框
					painter->setPen(QPen(QColor(Qt::gray), 2));                                                                               // 设置灰色边框
					painter->setBrush(QColor(Qt::gray));                                                                                      // 设置灰色填充
					painter->drawRoundedRect(Rc[1].adjusted(1, 1, -1, -1), 2, 2);

					painter->setPen(Qt::black);                                                                                               // 设置黑色文字
					painter->drawText(Rc[1], Qt::AlignCenter, QString("%1\%(%2/%3)").                                                         // 绵制测试进度文本
						arg((TestRes.GetLeft() * 100) / (TestRes.GetRight() + ((TestRes.GetRight() == 0) ? 1 : 0)), 3, 10).
						arg(TestRes.GetLeft(), 3, 10).arg(TestRes.GetRight(), 3, 10));
				}
				break;
				case NORMAL_ON_LINE:                                                                                                          // 正常在线状态转换为异常离线
					OnLineStat = ABNORMAL_OFF_LINE;                                                                                           // 状态转换
				case ABNORMAL_OFF_LINE:                                                                                                       // 异常离线状态
				{
					painter->fillRect(Rc[0], QColor(Qt::red));                                                                                // 状态指示区域填充红色
                                                                                                                                              //绘制背景边框
					painter->setPen(QPen(QColor(Qt::red), 2));                                                                                // 设置红色边框
					painter->setBrush(QColor(Qt::white));                                                                                     // 设置白色填充
					painter->drawRoundedRect(Rc[1].adjusted(1, 1, -1, -1), 2, 2);

                                                                                                                                              //绘制进度条
					QRect progressRect = Rc[1].adjusted(2, 2, -2, -2);
                                                                                                                                              // progressRect.setWidth(progressRect.width() * TestRes.GetLeft() / TestRes.GetRight());
					painter->fillRect(progressRect, QColor(Qt::red));                                                                         // 填充红色表示异常
					painter->setPen(Qt::black);                                                                                               // 设置黑色文字
					painter->drawText(Rc[1], Qt::AlignCenter, QString("%1\%(%2/%3)").                                                         // 绘制测试进度文本
						arg((TestRes.GetLeft() * 100) / (TestRes.GetRight() + ((TestRes.GetRight() == 0) ? 1 : 0)), 3, 10).
						arg(TestRes.GetLeft(), 3, 10).arg(TestRes.GetRight(), 3, 10));
				}
				break;
				case ABNORMAL_ON_LINE:                                                                                                        // 异常在线状态转换为异常离线
				{
					OnLineStat = ABNORMAL_OFF_LINE;                                                                                           // 状态转换
					painter->fillRect(Rc[0], QColor(Qt::red));                                                                                // 状态指示区域填充红色
                                                                                                                                              //绘制背景边框
					painter->setPen(QPen(QColor(Qt::red), 2));                                                                                // 设置红色边框
					painter->setBrush(QColor(Qt::white));                                                                                     // 设置白色填充
					painter->drawRoundedRect(Rc[1].adjusted(1, 1, -1, -1), 2, 2);

                                                                                                                                              //绘制进度条
					QRect progressRect = Rc[1].adjusted(2, 2, -2, -2);
                                                                                                                                              // progressRect.setWidth(progressRect.width() * TestRes.GetLeft() / TestRes.GetRight());
					painter->fillRect(progressRect, QColor(Qt::red));                                                                         // 填充红色表示异常
					painter->setPen(Qt::black);                                                                                               // 设置黑色文字
					painter->drawText(Rc[1], Qt::AlignCenter, QString("%1\%(%2/%3)").                                                         // 绘制测试进度文本
						arg((TestRes.GetLeft() * 100) / (TestRes.GetRight() + ((TestRes.GetRight() == 0) ? 1 : 0)), 3, 10).
						arg(TestRes.GetLeft(), 3, 10).arg(TestRes.GetRight(), 3, 10));
				}
				break;
				default:                                                                                                                      // 默认情况
					break;
				}
			}
                                                                                                                                              // palette.setColor(QPalette::Text, Qt::black);

			painter->drawText(Rc[0], Qt::AlignCenter, QString("S%1").arg(row));                                                               // 在第一个区域绘制芯片编号

                                                                                                                                              // ProgressBarOption.rect = Rc[1];
                                                                                                                                              // ProgressBarOption.minimum = 0;
                                                                                                                                              // ProgressBarOption.maximum = (TestRes.GetRight() == 0) ? 1 : TestRes.GetRight();
// ProgressBarOption.progress = TestRes.GetLeft();                                                                                            // QRandomGenerator::global()->bounded(0, 100);
                                                                                                                                              // WRITE_CENTRAL_WIDGET_DBG("Item::Paint(), TestRes.GetLeft() = %d, TestRes.GetRight() = %lld\n", TestRes.GetLeft(), TestRes.GetRight());
                                                                                                                                              // ProgressBarOption.text = QString("%1\%(%2/%3)").arg((TestRes.GetLeft() * 100) / (TestRes.GetRight() + ((TestRes.GetRight() == 0) ? 1 : 0)), 3, 10).arg(TestRes.GetLeft(), 3, 10).arg(TestRes.GetRight(), 3, 10);
                                                                                                                                              // ProgressBarOption.textAlignment = Qt::AlignCenter;
                                                                                                                                              // ProgressBarOption.textVisible = true;

                                                                                                                                              // ProgressBarOption.palette = palette;

                                                                                                                                              // QApplication::style()->drawControl(QStyle::CE_ProgressBar, &ProgressBarOption, painter);

			painter->fillRect(Rc[2], Qt::gray);                                                                                               // 第三个区域填充灰色
			painter->drawText(Rc[2], Qt::AlignCenter, QString("[%1, %2]").arg(SinadRang.GetLeft(), 2, 10).arg(SinadRang.GetRight(), 2, 10));  // 显示SINAD范围

			painter->restore();                                                                                                               // 恢复画笔状态

                                                                                                                                              // WRITE_CENTRAL_WIDGET_DBG("Item::paint(), End\n");
		}
	}

	/************************* Model类：Qt模型视图框架的数据模型实现 *************************/

	Model::Model(QObject* parent)
		:QAbstractListModel(parent)                                                                                                           // 继承自QAbstractListModel
	{
	}

	void Model::AddItem(Item i)
	{
		beginInsertRows(QModelIndex(), rowCount(), rowCount());                                                                               // 通知视图开始插入行
		Items.push_back(i);                                                                                                                   // 添加项目到容器
		endInsertRows();                                                                                                                      // 通知视图结束插入行
	}

	void Model::UpdateItem(int row, Item i)
	{
		Items[row - 1] = i;                                                                                                                   // 更新指定行的项目数据
		QModelIndex index = createIndex(row, 0);                                                                                              // 创建模型索引
		dataChanged(index, index);                                                                                                            // 通知视图数据已改变
	}

	void Model::UpdateItemOnlineStatus(int row, bool online)
	{
		if (row > 0 && row <= Items.size()) {                                                                                                 // 检查行索引有效性
			Items[row - 1].IfValid = online;                                                                                                  // 更新在线状态
			QModelIndex index = createIndex(row, 0);                                                                                          // 创建模型索引
			dataChanged(index, index);                                                                                                        // 通知视图数据已改变
		}
	}

	void Model::SetBoardNum(int n)
	{
		BoardNum = n;                                                                                                                         // 设置板卡编号
	}

	int Model::rowCount(const QModelIndex& parent) const
	{
		return static_cast<int>(Items.size()) + 1;                                                                                            // 返回行数，+1是为表头预留
	}

	QVariant Model::data(const QModelIndex& index, int role) const
	{
		if (index.row() > 0 && index.row() < rowCount())                                                                                      // 数据行
		{
			switch (role)                                                                                                                     // 根据角色返回不同数据
			{
			case ITEM:              return QVariant::fromValue(Items[index.row() - 1]);                                                       // 返回完整项目
			case IF_VALID:          return Items[index.row() - 1].IfValid;                                                                    // 返回有效标志
			case TEST_RES_RANG:     return QVariant::fromValue(Items[index.row() - 1].TestRes);                                               // 返回测试结果范围
			case SINAD_RANGE:       return QVariant::fromValue(Items[index.row() - 1].SinadRang);                                             // 返回SINAD范围
			default:
				break;
			}
		}
		else                                                                                                                                  // 表头行
		{
			return QString("J%1").arg(BoardNum);                                                                                              // 返回板卡编号
		}

		return QVariant();                                                                                                                    // 默认返回空值
	}

	bool Model::setData(const QModelIndex& index, const QVariant& value, int role)
	{
		if (index.row() > 0 && index.row() < rowCount())                                                                                      // 只对数据行进行设置
		{
			switch (role)                                                                                                                     // 根据角色设置不同数据
			{
			case ITEM:          Items[index.row() - 1] = value.value<Item>();                return true;                                     // 设置完整项目
			case IF_VALID:      Items[index.row() - 1].IfValid = value.value<bool>();        return true;                                     // 设置有效标志
			case TEST_RES_RANG: Items[index.row() - 1].TestRes = value.value<Range>();       return true;                                     // 设置测试结果范围
			case SINAD_RANGE:   Items[index.row() - 1].SinadRang = value.value<Range>();     return true;                                     // 设置SINAD范围
			default:
				break;
			}
		}

		return false;                                                                                                                         // 设置失败
	}

	Qt::ItemFlags Model::flags(const QModelIndex& index) const
	{
		if (index.row() > 0 && index.row() < rowCount())                                                                                      // 数据行
		{
			return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;                                                             // 可编辑、可用、可选择
		}
		return Qt::ItemIsEnabled;                                                                                                             // 表头行只可用
	}

	/************************* Delegate类：Qt模型视图框架的委托实现 *************************/

	void Delegate::SetBoardNum(int n)
	{
		BoardNum = n;                                                                                                                         // 设置板卡编号
	}

	void Delegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		if (index.row() > 0)                                                                                                                  // 判断是否为数据行
		{
			Item item = index.data(Model::ITEM).value<Item>();                                                                                // 获取当前行的数据Item对象
			item.Paint(painter, option, index.row());                                                                                         // 调用Item的自定义绘制方法
			QStyledItemDelegate::paint(painter, option, index);                                                                               // 保证选中等默认样式正常显示
		}
		else if (index.row() == 0)                                                                                                            // 判断是否为表头行
		{
			painter->fillRect(option.rect, QBrush(QColor(255, 255, 255)));                                                                    // 绘制表头背景为白色
			QFont font("Microsoft YaHei");                                                                                                    // 设置表头字体
			font.setWeight(QFont::Black);                                                                                                     // 设置字体加粗
			painter->setFont(font);                                                                                                           // 应用字体
			painter->drawText(option.rect, Qt::AlignLeft, QString("J%1").arg(BoardNum));                                                      // 绘制表头文本，显示板卡编号
		}
	}

	QSize Delegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		if (index.row() > 0)                                                                                                                  // 数据行
		{
			return QSize(option.rect.width(), 24);                                                                                            // 返回24像素高度
		}
		else if (index.row() == 0)                                                                                                            // 表头行
		{
			return QSize(option.rect.width(), 24);                                                                                            // 返回24像素高度
		}
		return QSize();                                                                                                                       // 默认尺寸
	}

	/************************* RetEventFilter类：回车键事件过滤器实现 *************************/

	bool RetEventFilter::eventFilter(QObject* obj, QEvent* e)
	{
		switch (e->type())                                                                                                                    // 根据事件类型处理
		{
		case QEvent::KeyPress:                                                                                                                // 按键按下事件
		{
			QKeyEvent* KeyEvent = static_cast<QKeyEvent*>(e);                                                                                 // 转换为按键事件
			switch (KeyEvent->key())                                                                                                          // 根据按键类型处理
			{
			case Qt::Key_Return:                                                                                                              // 回车键
			case Qt::Key_Enter:                                                                                                               // 小键盘回车键
			{
				QPlainTextEdit* TextEdit = qobject_cast<QPlainTextEdit*>(obj);                                                                // 尝试转换为文本编辑控件
				if (TextEdit)                                                                                                                 // 如果转换成功
				{
					TextEdit->clearFocus();                                                                                                   // 清除焦点
					return true;                                                                                                              // 事件已处理
				}
			}
			break;

			default:                                                                                                                          // 其他按键
				break;
			}
		}
		break;

		default:                                                                                                                              // 其他事件类型
			break;
		}

		return QObject::eventFilter(obj, e);                                                                                                  // 调用基类处理
	}

	/************************* CentralWidget类：中央窗口主控件实现 *************************/

	CentralWidget::CentralWidget(QWidget* parent)
		:QWidget(parent), isConnected(false)                                                                                                 // 继承自QWidget，初始化连接状态为false
	{
		OPEN_CENTRAL_WIDGWT_DBG(LogCenTralWidget.txt);                                                                                        // 打开调试日志

		InitUI();                                                                                                                             // 初始化用户界面
		InitThread();                                                                                                                         // 初始化线程
		Connect();                                                                                                                            // 连接信号槽
		CreateThread();                                                                                                                       // 创建工作线程
		
		// 新增：连接全局信息管理器信号到当前实例
		connect(InfoMessageManager::getInstance(), &InfoMessageManager::infoMessageReady,
			this, &CentralWidget::OnGlobalInfoMessage, Qt::QueuedConnection);
		
		// 检查是否已经有网络连接，如果有则启动定时检测
		// 这里可以添加检查当前连接状态的逻辑
                                                                                                                                              // TimingDetection->start(1000);
	}

	CentralWidget::~CentralWidget()
	{
		if (TaskStatistics) TaskStatistics = nullptr;                                                                                         // 释放统计任务指针
		if (TaskSend) TaskSend = nullptr;                                                                                                     // 释放发送任务指针
		TimingDetection->stop();                                                                                                              // 停止定时检测
		CLOSE_CENTRAL_WIDGET_DBG();                                                                                                           // 关闭调试日志
	}

	/************************* 新增：重置在线芯片状态为蓝色显示的方法 *************************/
	void CentralWidget::ResetOnlineChipsToBlue()
	{
		for (int i = 0; i < 8; i++)                                                                                                           // 遍历8块板卡
		{
			for (int j = 0; j < 4; j++)                                                                                                       // 每块板卡4个芯片
			{
				if (DCR::DeviceCheckResultGlobal->GetChipCheckResult(i, j).GetIfOnline())                                                     // 如果芯片在线
				{
					// 重置在线状态为初始离线状态，这样在Paint方法中会转换为蓝色显示
					Models[i].UpdateItem(j + 1, CWD::Item(
						true,                                                                                                                 // 芯片有效（在线）
						Range(0, 0),                                                                                                          // 测试结果范围重置为0/0，触发蓝色显示
						Range(DCR::DeviceCheckResultGlobal->GetChipCheckResult(i, j).GetRangeSINAD())                                         // 保持当前SINAD范围
					));
					
					// 重置芯片的在线状态为初始状态，确保下次Paint时会显示蓝色
					Models[i].Items[j].RestOnLineStat();
				}
			}
		}
		
		AppendInfoWithTime("在线芯片界面已重置为蓝色显示", "INFO");
	}

	/************************* 芯片在线状态更新方法：定时刷新所有芯片的在线状态 *************************/
   /* void CentralWidget::UpdateChipOnlineStatus()                                                                                            // 注释掉的旧版本实现
	{
		for (int i = 0; i < 8; i++)                                                                                                           // 遍历8块板卡
		{
			for (int j = 0; j < 4; j++)                                                                                                       // 每块板卡4个芯片
			{
				Models[i].UpdateItem(j + 1, CWD::Item(
					DCR::DeviceCheckResultGlobal->GetChipCheckResult(i, j).GetIfOnline(),                                                     // 芯片是否在线
					Range(
						DCR::DeviceCheckResultGlobal->GetChipCheckResult(i, j).GetCheckSatisfiedCount(),
						DCR::DeviceCheckResultGlobal->GetCheckCompletedCount()
					),                                                                                                                        // 测试结果范围
					Range(DCR::DeviceCheckResultGlobal->GetChipCheckResult(i, j).GetRangeSINAD())                                             // SINAD范围
				));
			}
		}
		POnlineChipNum.setText(QString("%1").arg(DCR::DeviceCheckResultGlobal->GetChipOnLineNum()));                                          // 更新在线芯片数量显示
	}*/

	void CentralWidget::UpdateChipOnlineStatus()
	{
		for (int i = 0; i < 8; i++)                                                                                                           // 遍历8块板卡
		{
			for (int j = 0; j < 4; j++)                                                                                                       // 每块板卡4个芯片
			{
				Models[i].UpdateItemOnlineStatus(j + 1,                                                                                       // 更新指定芯片的在线状态
					DCR::DeviceCheckResultGlobal->GetChipCheckResult(i, j).GetIfOnline());                                                    // 获取芯片在线状态
			}
		}
		POnlineChipNum.setText(QString("%1").arg(DCR::DeviceCheckResultGlobal->GetChipOnLineNum()));                                          // 更新在线芯片数量显示
	}

	/************************* 用户界面初始化方法：构建整个界面布局 *************************/
	void CentralWidget::InitUI()
	{
		// 标签名
		QLabel* labelLocalIP = new QLabel("本机IP", this);
		QLabel* labelDeviceIP = new QLabel("设备IP", this);
		QLabel* labelVersion = new QLabel("版本号", this);
		QLabel* labelChipType = new QLabel("芯片型号", this);
		QLabel* labelTempInner = new QLabel("FPGA温度", this);
		QLabel* labelTempEnv = new QLabel("环境温度", this);

		// 冒号
		QLabel* colon1 = new QLabel(":", this);
		QLabel* colon2 = new QLabel(":", this);
		QLabel* colon3 = new QLabel(":", this);
		QLabel* colon4 = new QLabel(":", this);
		QLabel* colon5 = new QLabel(":", this);
		QLabel* colon6 = new QLabel(":", this);

		// 初始化版本和芯片信息标签
		VersionNum.setParent(this);
		ChipType.setParent(this);
		TemperatureInner.setParent(this);
		TemperatureEnv.setParent(this);
		
		// 初始化网络配置输入框
		LeLocalIP.setParent(this);
		LeLocalPort.setParent(this);
		LeDeviceIP.setParent(this);
		LeDevicePort.setParent(this);
		BtnConnect.setParent(this);

		VersionNum.setText("--");
		ChipType.setText("AIC9628BM");
		TemperatureInner.setText("--℃");
		TemperatureEnv.setText("--℃");

		// 统一宽度设置
		int labelWidth = 60;   // 标签宽度
		int colonWidth = 8;    // 冒号宽度
		int ipWidth = 100;     // IP输入框宽度
		int portWidth = 50;    // 端口输入框宽度
		int infoWidth = 150;   // 信息显示宽度
		int btnWidth = 50;     // 按钮宽度
		
		// 设置控件宽度
		labelLocalIP->setFixedWidth(labelWidth);
		labelDeviceIP->setFixedWidth(labelWidth);
		labelVersion->setFixedWidth(labelWidth);
		labelChipType->setFixedWidth(labelWidth);
		labelTempInner->setFixedWidth(labelWidth);
		labelTempEnv->setFixedWidth(labelWidth);

		colon1->setFixedWidth(colonWidth);
		colon2->setFixedWidth(colonWidth);
		colon3->setFixedWidth(colonWidth);
		colon4->setFixedWidth(colonWidth);
		colon5->setFixedWidth(colonWidth);
		colon6->setFixedWidth(colonWidth);
		
		LeLocalIP.setFixedWidth(ipWidth);
		LeLocalPort.setFixedWidth(portWidth);
		LeDeviceIP.setFixedWidth(ipWidth);
		LeDevicePort.setFixedWidth(portWidth);
		BtnConnect.setFixedWidth(btnWidth);
		
		VersionNum.setFixedWidth(infoWidth);
		ChipType.setFixedWidth(infoWidth);
		TemperatureInner.setFixedWidth(infoWidth);
		TemperatureEnv.setFixedWidth(infoWidth);

		// 内容左对齐
		VersionNum.setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		ChipType.setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		TemperatureInner.setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		TemperatureEnv.setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

		// 设置标签样式
		QString labelStyle = "QLabel { color: #333333; font-weight: normal; }";
		QString colonStyle = "QLabel { color: #666666; font-weight: normal; }";
		
		labelLocalIP->setStyleSheet(labelStyle);
		labelDeviceIP->setStyleSheet(labelStyle);
		labelVersion->setStyleSheet(labelStyle);
		labelChipType->setStyleSheet(labelStyle);
		labelTempInner->setStyleSheet(labelStyle);
		labelTempEnv->setStyleSheet(labelStyle);
		
		colon1->setStyleSheet(colonStyle);
		colon2->setStyleSheet(colonStyle);
		colon3->setStyleSheet(colonStyle);
		colon4->setStyleSheet(colonStyle);
		colon5->setStyleSheet(colonStyle);
		colon6->setStyleSheet(colonStyle);
		
		// 设置输入框样式
		QString inputStyle = 
			"QLineEdit {"
			"    border: 1px solid #d0d0d0;"
			"    border-radius: 3px;"
			"    padding: 2px 4px;"
			"    font-size: 12px;"
			"    background-color: white;"
			"}"
			"QLineEdit:focus {"
			"    border: 1px solid #4CAF50;"
			"}";
			
		LeLocalIP.setStyleSheet(inputStyle);
		LeLocalPort.setStyleSheet(inputStyle);
		LeDeviceIP.setStyleSheet(inputStyle);
		LeDevicePort.setStyleSheet(inputStyle);

		// 初始化网络配置相关
		InitNetworkConfig();

		// 第一行：网络配置 (本机IP : xxx  端口框  设备IP : xxx  端口框  连接按钮)
		QHBoxLayout* row1 = new QHBoxLayout;
		row1->addWidget(labelLocalIP);
		row1->addWidget(colon1);
		row1->addWidget(&LeLocalIP);
		row1->addSpacing(5);
		row1->addWidget(&LeLocalPort);
		row1->addSpacing(30);  // 本机和设备之间的间距
		row1->addWidget(labelDeviceIP);
		row1->addWidget(colon2);
		row1->addWidget(&LeDeviceIP);
		row1->addSpacing(5);
		row1->addWidget(&LeDevicePort);
		row1->addSpacing(20);
		row1->addWidget(&BtnConnect);
		row1->addStretch();

		// 第二行：版本号和芯片型号 (版本号 : xxx        芯片型号 : xxx)
		QHBoxLayout* row2 = new QHBoxLayout;
		row2->addWidget(labelVersion);
	 row2->addWidget(colon3);
	 row2->addWidget(&VersionNum);
	 row2->addSpacing(40);  // 版本号和芯片型号之间的间距
	 row2->addWidget(labelChipType);
	 row2->addWidget(colon4);
	 row2->addWidget(&ChipType);
	 row2->addStretch();

		// 第三行：FPGA温度和环境温度 (FPGA温度 : xxx℃    环境温度 : xxx℃)
		QHBoxLayout* row3 = new QHBoxLayout;
		row3->addWidget(labelTempInner);
	 row3->addWidget(colon5);
	 row3->addWidget(&TemperatureInner);
	 row3->addSpacing(40);  // FPGA温度和环境温度之间的间距
	 row3->addWidget(labelTempEnv);
	 row3->addWidget(colon6);
	 row3->addWidget(&TemperatureEnv);
	 row3->addStretch();

		// 整体状态信息布局
		QVBoxLayout* statLayout = new QVBoxLayout;
		statLayout->addLayout(row1);
		statLayout->addLayout(row2);
		statLayout->addLayout(row3);
		statLayout->setContentsMargins(8, 8, 8, 8);  
		statLayout->setSpacing(6);  // 行间距

		GbStat.setLayout(statLayout);

		for (int i = 0; i < 8; i++)
		{
			Models[i].SetBoardNum(i + 1);
			Models[i].AddItem(Item(false, Range(0, 0), Range(-99, -99)));
			Models[i].AddItem(Item(false, Range(0, 0), Range(-99, -99)));
			Models[i].AddItem(Item(false, Range(0, 0), Range(-99, -99)));
			Models[i].AddItem(Item(false, Range(0, 0), Range(-99, -99)));

			Delegates[i].SetBoardNum(i + 1);
			Lists[i].setModel(&Models[i]);
			Lists[i].setItemDelegate(&Delegates[i]);
		}

		Filter = new RetEventFilter(this);

		LTestNum.setText("总测试次数");
		LTestedNum.setText("已测试");
		LTestTime.setText("总测试时间");
		LOnlineChipNum.setText("芯片在线数量");
		LSatisfiedChipNum.setText("测试通过数量");
		LRejectionChipNum.setText("测试失败数量");
		BtnStartTest.setText("开始测试");
		BtnOpenLog.setText("打开日志");
		PTestNum.installEventFilter(Filter);
		PTestNum.setPlainText("1"); // 设置总测试次数初始值为1
		PTestNum.setMaximumHeight(20);
		PTestedNum.setMaximumHeight(20);
		PTestTime.setMaximumHeight(20);
		POnlineChipNum.setMaximumHeight(20);
		PSatisfiedChipNum.setMaximumHeight(20);
		PRejectionChipNum.setMaximumHeight(20);

		PTestNum.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		PTestNum.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		PTestNum.setMaximumWidth(40);
		PTestedNum.setMaximumWidth(60);
		PTestTime.setMaximumWidth(60);
		POnlineChipNum.setMaximumWidth(60);
		PSatisfiedChipNum.setMaximumWidth(60);
		PRejectionChipNum.setMaximumWidth(60);

		HlTestNum.addWidget(&LTestNum);
		HlTestNum.addStretch();
		HlTestNum.addWidget(&PTestNum);
		HlTestNum.addSpacing(5);
		HlTestedNum.addWidget(&LTestedNum);
		HlTestedNum.addStretch();
		HlTestedNum.addWidget(&PTestedNum);
		HlTestedNum.addSpacing(5);
		HlTestTime.addWidget(&LTestTime);
		HlTestTime.addStretch();
		HlTestTime.addWidget(&PTestTime);
		HlTestTime.addSpacing(5);
		HlOnlineChipNum.addWidget(&LOnlineChipNum);
		HlOnlineChipNum.addStretch();
		HlOnlineChipNum.addWidget(&POnlineChipNum);
		HlOnlineChipNum.addSpacing(5);
		HlSatisfiedNum.addWidget(&LSatisfiedChipNum);
		HlSatisfiedNum.addStretch();
		HlSatisfiedNum.addWidget(&PSatisfiedChipNum);
		HlSatisfiedNum.addSpacing(5);
		HlRejectedNum.addWidget(&LRejectionChipNum);
		HlRejectedNum.addStretch();
		HlRejectedNum.addWidget(&PRejectionChipNum);
		HlRejectedNum.addSpacing(5);
		HlBtn.addStretch();
		HlBtn.addWidget(&BtnStartTest);
		HlBtn.addStretch();
		HlBtn.addWidget(&BtnOpenLog);
		HlBtn.addStretch();
		VlInstrumentBoard.addLayout(&HlTestNum);
		VlInstrumentBoard.addLayout(&HlTestedNum);
		VlInstrumentBoard.addLayout(&HlTestTime);
		VlInstrumentBoard.addLayout(&HlOnlineChipNum);
		VlInstrumentBoard.addLayout(&HlSatisfiedNum);
		VlInstrumentBoard.addLayout(&HlRejectedNum);
		VlInstrumentBoard.addLayout(&HlBtn);
		VlInstrumentBoard.addStretch();
		VlInstrumentBoard.setContentsMargins(8, 8, 8, 8);
		VlInstrumentBoard.setSpacing(4);
		GbInstrumentBoard.setLayout(&VlInstrumentBoard);

		HlLineFst.addWidget(&Lists[0]);
		HlLineFst.addWidget(&Lists[1]);
		HlLineFst.addWidget(&Lists[2]);
		HlLineSec.addWidget(&Lists[7]);
		HlLineSec.addWidget(&GbInstrumentBoard);
		HlLineSec.addWidget(&Lists[3]);
		HlLineThd.addWidget(&Lists[6]);
		HlLineThd.addWidget(&Lists[5]);
		HlLineThd.addWidget(&Lists[4]);
		VlDevStatistics.addLayout(&HlLineFst);
		VlDevStatistics.addLayout(&HlLineSec);
		VlDevStatistics.addLayout(&HlLineThd);
		VlDevStatistics.setContentsMargins(5, 5, 5, 5);
		VlDevStatistics.setSpacing(3);
		GbDevStatistics.setLayout(&VlDevStatistics);

		// 新增：初始化底部信息显示区域
		InitInfoArea();

		VlAll.addWidget(&GbStat);
		VlAll.addWidget(&GbDevStatistics);
		VlAll.addWidget(&GbInfoArea);
		
		// 优化主布局的间距和边距
		VlAll.setContentsMargins(5, 5, 5, 5);
		VlAll.setSpacing(3);

		setLayout(&VlAll);

		Clock = new QTimer(this);
		TimingDetection = new QTimer(this);
		TotalGroupCount = CFGI::IniFileCFGGlobal->ReadINI(CFGI::INI_TYPE::INI_CENTRALIZE, "NUMBER/rowCount").toInt();
	}

	/************************* 线程初始化，创建任务等 *************************/
	void CentralWidget::InitThread()
	{
		WRITE_CENTRAL_WIDGET_DBG("InitThread()\n");
		TaskStatistics = new TASKWZ::TaskTestStatistics;
		TaskSend = nullptr;
		TaskVersion = new TASKWZ::TaskVersionParsing;
	}

	/************************* 信号与槽连接 *************************/
	void CentralWidget::Connect()
	{
		connect(&BtnStartTest, &QPushButton::clicked, this, &CentralWidget::StartTest);
		connect(&BtnOpenLog, &QPushButton::clicked, this, &CentralWidget::OpenLog);
		connect(TaskStatistics, &TASKWZ::TaskTestStatistics::MsgOfStatistics, this, &CentralWidget::UpdateCheckResult, Qt::QueuedConnection);
		connect(TaskVersion, &TASKWZ::TaskVersionParsing::UpdateBoardVersion, this, &CentralWidget::UpdateVersion, Qt::QueuedConnection);
		connect(Clock, &QTimer::timeout, this, &CentralWidget::ClockEvent);
		connect(TimingDetection, &QTimer::timeout, this, &CentralWidget::TimingDetectionEvent);
		
		// 新增：信息显示区域信号槽连接
		connect(&BtnClearInfo, &QPushButton::clicked, this, &CentralWidget::OnClearInfo);
		connect(&BtnSaveInfo, &QPushButton::clicked, this, &CentralWidget::OnSaveInfo);
		connect(&ChkAutoScroll, &QCheckBox::toggled, this, &CentralWidget::OnAutoScrollToggled);
		
		// 新增：网络配置信号槽连接
		connect(&BtnConnect, &QPushButton::clicked, this, &CentralWidget::OnConnectBtnClicked);
		connect(&LeLocalIP, &QLineEdit::textChanged, this, &CentralWidget::OnNetworkInputChanged);
		connect(&LeLocalPort, &QLineEdit::textChanged, this, &CentralWidget::OnNetworkInputChanged);
		connect(&LeDeviceIP, &QLineEdit::textChanged, this, &CentralWidget::OnNetworkInputChanged);
		connect(&LeDevicePort, &QLineEdit::textChanged, this, &CentralWidget::OnNetworkInputChanged);
	}

	/************************* 创建线程与任务 *************************/
	void CentralWidget::CreateThread()
	{
		TASKWZ::worker_manager::create(TaskStatistics, TASKWZ::worker_type::EXECUTE_THREAD);
		CREATE_TASK_MNIC_PARSING;
		CREATE_TASK_DISPATCH;
		CREATE_TASK_RCV;
		TASKWZ::worker_manager::create(TaskVersion, TASKWZ::worker_type::EXECUTE_THREAD);
	}

	/************************* 开始测试槽函数：启动测试流程 *************************/
	void CentralWidget::StartTest()
	{
		WRITE_CENTRAL_WIDGET_DBG("StartTest(), Enter\n");                                                                                     // 写调试日志，记录进入StartTest函数

		if (StatOfBtnStart)                                                                                                                   // 如果当前是"开始测试"状态（按钮未被按下，准备开始测试）
		{
                                                                                                                                              // 只在测试开始时生成一次时间戳
			testSessionTime = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
			currentLogSePath = QCoreApplication::applicationDirPath() + "/LOG/LogSeRecord_" + testSessionTime + ".log";
			LWZ::Log::LogInstance->Init(LOG_SE_RECORD_INDEX, currentLogSePath.toStdString().c_str());

			LWZ::Log::LogInstance->Init(LOG_SE_RECORD_INDEX, currentLogSePath.toStdString().c_str());                                         //新增：生成并记录日志文件名

		//	DCR::DeviceCheckResultGlobal->SetCheckedGroupCount(0);
		///*	DCR::DeviceCheckResultGlobal->SetPassedGroupCount(0);
		//	DCR::DeviceCheckResultGlobal->SetTotalGroupCount(0);*/

			// 新增：添加测试开始信息
			AppendInfoWithTime(QString("开始测试，计划测试 %1 次").arg(PTestNum.toPlainText()), "INFO");
			AppendInfoWithTime(QString("简要日志文件: %1").arg(currentLogSePath), "INFO");

			// 新增：重置在线芯片界面为蓝色显示
			ResetOnlineChipsToBlue();

			//PTestedNum.setText("0(0/0)");                                                                                                    // 已测试次数清零，界面显示为0
			PTestedNum.setText("");                                                                                                            // 已测试次数清零
			PTestTime.setText("");                                                                                                             // 测试时间清零
			PSatisfiedChipNum.setText("");                                                                                                    // 测试通过数量清零
			PRejectionChipNum.setText("");                                                                                                    // 测试失败数量清零
			TOOLWZ::stack_wz<TASKWZ::task_type> stack_join;                                                                                   // 创建一个任务类型栈，需要批量join的任务
			stack_join.push(TASKWZ::task_type::TASK_END);                                                                                     // 压入"结束任务"类型
			stack_join.push(TASKWZ::task_type::TASK_JOIN);                                                                                    // 压入"join任务"类型
			stack_join.push(TASKWZ::TASK_DATA_CONSTRUCT);                                                                                     // 压入"数据构建任务"类型
		 stack_join.push(TASKWZ::TASK_DATA_SEND);                                                                                             // 压入"数据发送任务"类型
			CREATE_TASK_JOIN(stack_join);                                                                                                     // 创建一个join任务，批量等待上述所有类型的任务完成
			TaskSend = new TASKWZ::TaskDataSend(PTestNum.toPlainText().toInt());                                                              // 创建数据发送任务对象，参数为界面输入的测试次数
			connect(TaskSend, &TASKWZ::TaskDataSend::MsgOfStartEnd, this, &CentralWidget::UpdateCheckResult, Qt::QueuedConnection);           // 连接TaskSend的"测试开始/结束"信号到界面结果更新槽函数
			TASKWZ::worker_manager::create(TaskSend, TASKWZ::worker_type::EXECUTE_THREAD);                                                    // 将TaskSend任务交给任务管理器调度（以线程方式执行）
			StatOfBtnStart = false;                                                                                                           // 状态切换为"测试中"
			BtnStartTest.setText("结束测试");                                                                                                 // 按钮文本切换为"结束测试"
		}
		else
		{
			// 新增：添加测试结束信息
			AppendInfoWithTime("用户手动结束测试", "WARNING");
			
			std::unique_lock lock(mtx);                                                                                                       // 加锁，保证线程安全
			Clock->stop();                                                                                                                    // 停止计时器
			if (TaskSend)                                                                                                                     // 如果TaskSend存在，断开信号连接，关闭任务并释放对象
			{
				disconnect(TaskSend, &TASKWZ::TaskDataSend::MsgOfStartEnd, this, &CentralWidget::UpdateCheckResult);
				TaskSend->close();
				//delete TaskSend;                                                                                                            // 新增：释放内存
				TaskSend = nullptr;
			}

			TOOLWZ::stack_wz<TASKWZ::task_type> stack_end;                                                                                    // 创建一个任务类型栈，用于批量结束任务

			stack_end.push(TASKWZ::TASK_DATA_SEND);                                                                                           // 压入"数据发送任务"类型

			CREATE_TASK_END(stack_end);                                                                                                       // 创建一个end任务，批量关闭上述类型的任务

			StatOfBtnStart = true;                                                                                                            // 状态切换为"未测试"
			BtnStartTest.setText("开始测试");                                                                                                     // 按钮文本切换为"开始测试"

                                                                                                                                              // 结束时用同一个时间戳生成处理后日志名
			QString processedLogPath = QString("./LOG/LogUpRecordProcessed_%1.log").arg(testSessionTime);
			LPWZ::LogProcessor().processLog("./LOG/LogUpRecord.log", processedLogPath.toStdString());
			
			// 新增：添加日志处理完成信息
			AppendInfoWithTime(QString("日志处理完成: %1").arg(processedLogPath), "SUCCESS");

			lock.unlock();                                                                                                                    // 解锁
		}
		WRITE_CENTRAL_WIDGET_DBG("StartTest(), End\n");                                                                                       // 写调试日志，记录离开StartTest函数
	}

	/***********************打开简要日志LogSeRecord.log*********************************/
	void CentralWidget::OpenLog()
	{
		if (currentLogSePath.isEmpty()) {
			QMessageBox::warning(this, "提示", "本次测试日志路径未知！");
			return;
		}
		QFileInfo fi(currentLogSePath);
		if (fi.exists()) {
			QDesktopServices::openUrl(QUrl::fromLocalFile(currentLogSePath));
		}
		else {
			QMessageBox::warning(this, "提示", "日志文件不存在！");
		}
	}

	/************************* 更新检查结果槽函数：处理测试开始、进行中、结束的状态消息 *************************/
	void CentralWidget::UpdateCheckResult(TASKWZ::MsgToCentralWt msg)
	{
		WRITE_CENTRAL_WIDGET_DBG("CentralWidget::UpdateCheckResult(), Enter\n");                                                              // 记录进入函数

		switch (msg.GetStat())                                                                                                                // 根据测试状态分支
		{
		case TASKWZ::TestStat::TEST_START:
			ClockVal = 0;                                                                                                                     // 计时器数值清零
			Clock->start(1000);                                                                                                               // 启动计时器，每秒触发
			DCR::DeviceCheckResultGlobal->Init();                                                                                             // 初始化检测结果
			DCR::DeviceCheckResultGlobal->SetCheckCount(TaskSend->TestCount);                                                                 // 设置本次测试总次数
			// 新增：添加测试开始信息
			AppendInfoWithTime("测试流程正式开始", "SUCCESS");
			TASKWZ::TaskDataSend::SemaWaitForUI.release(1);                                                                                   // 通知UI可以继续
			break;
		case TASKWZ::TestStat::TEST_RUNNING:
			WRITE_CENTRAL_WIDGET_DBG("case TESTRUNNING\n");                                                                                   // 记录调试信息
			switch (msg.GetMode())                                                                                                            // 根据统计模式分支
			{
			case TASKWZ::STATISTICS_A_MIF:
				WRITE_CENTRAL_WIDGET_DBG("case: STATISTICS_A_MIF Enter\n");                                                                   // 记录调试信息
				for (int i = 0; i < 8; i++)                                                                                                   // 遍历8块板卡
				{
					for (int j = 0; j < 4; j++)                                                                                               // 每块4个芯片
					{
						Models[i].UpdateItem(j + 1, CWD::Item(
							DCR::DeviceCheckResultGlobal->GetChipCheckResult(i, j).GetIfOnline(),                                             // 芯片是否在线
							Range(DCR::DeviceCheckResultGlobal->GetChipCheckResult(i, j).GetPassedGroupCount(),
								DCR::DeviceCheckResultGlobal->GetTotalGroupCount()),                                                      // 该芯片的通过组数/总组数
							Range(DCR::DeviceCheckResultGlobal->GetChipCheckResult(i, j).GetRangeSINAD())                                     // SINAD范围
						));
						//DCR::DeviceCheckResultGlobal->GetChipCheckResult(i, j).SetRangeSINAD(TCOND::Range(0, 0));
						//DCR::DeviceCheckResultGlobal->GetChipCheckResult(i, j).SetCheckPacksOfMif(0,0);                                     // MIF包计数清零
						//DCR::DeviceCheckResultGlobal->GetChipCheckResult(i, j).SetChipTestStat(DCR::WAITING_FOR_TESTING);                   // 状态设为等待测试
					}
				}
				WRITE_CENTRAL_WIDGET_DBG("for(for()) after\n");                                                                               // 记录调试信息
				POnlineChipNum.setText(QString("%1").arg(DCR::DeviceCheckResultGlobal->GetChipOnLineNum()));                                  // 更新在线芯片数
				PSatisfiedChipNum.setText(QString("%1").arg(DCR::DeviceCheckResultGlobal->GetChipSatisfiedNum()));                            // 更新通过芯片数
				PRejectionChipNum.setText(QString("%1").arg(DCR::DeviceCheckResultGlobal->GetChipUnSatisfiedNum()));                          // 更新失败芯片数
				//// 新增：添加测试状态信息
				//AppendInfo(QString("数据统计完成 - 在线:%1 通过:%2 失败:%3")
				//	.arg(DCR::DeviceCheckResultGlobal->GetChipOnLineNum())
				//	.arg(DCR::DeviceCheckResultGlobal->GetChipSatisfiedNum())
				//	.arg(DCR::DeviceCheckResultGlobal->GetChipUnSatisfiedNum()), "INFO");
				TASKWZ::TaskDataSend::SemaWaitForUI.release(1);                                                                               // 通知UI可以继续
				WRITE_CENTRAL_WIDGET_DBG("case: STATISTICS_A_MIF Leave\n");                                                                   // 记录调试信息
				break;
			case TASKWZ::STATISTICS_A_TIME:
				WRITE_CENTRAL_WIDGET_DBG("case: STATISTICS_A_TIME\n");                                                                        // 记录调试信息
				for (int i = 0; i < 8; i++)                                                                                                   // 遍历8块板卡
				{
					for (int j = 0; j < 4; j++)                                                                                               // 每块4个芯片
					{
						Models[i].UpdateItem(j + 1, CWD::Item(
							DCR::DeviceCheckResultGlobal->GetChipCheckResult(i, j).GetIfOnline(),                                             // 芯片是否在线
							Range(DCR::DeviceCheckResultGlobal->GetChipCheckResult(i, j).GetPassedGroupCount(),
								DCR::DeviceCheckResultGlobal->GetTotalGroupCount()),                                                  // 该芯片的通过组数/总组数
							Range(DCR::DeviceCheckResultGlobal->GetChipCheckResult(i, j).GetRangeSINAD())                                     // SINAD范围
						));
						//DCR::DeviceCheckResultGlobal->GetChipCheckResult(i, j).SetRangeSINAD(TCOND::Range(0, 0));                             //新增：SINAD范围清零
					}
				}
                                                                                                                                              //PTestedNum.setText(QString("%1(%2/%3)")
                                                                                                                                              //    .arg(DCR::DeviceCheckResultGlobal->GetCheckCompletedCount() + 1)
                                                                                                                                              //    .arg(DCR::DeviceCheckResultGlobal->GetCheckedGroupCount() + 1)
//    .arg(CFGI::IniFileCFGGlobal->ReadINI(CFGI::INI_TYPE::INI_CENTRALIZE, "NUMBER/rowCount").toInt()));                                      // 更新已测试次数
				POnlineChipNum.setText(QString("%1").arg(DCR::DeviceCheckResultGlobal->GetChipOnLineNum()));                                  // 更新在线芯片数
				//PSatisfiedChipNum.setText(QString("%1").arg(DCR::DeviceCheckResultGlobal->GetChipSatisfiedNum()));                          // 更新通过芯片数
				//PRejectionChipNum.setText(QString("%1").arg(DCR::DeviceCheckResultGlobal->GetChipUnSatisfiedNum()));                        // 更新失败芯片数
				DCR::DeviceCheckResultGlobal->Reset();                                                                                        // 重置检测结果
				DCR::DeviceCheckResultGlobal->CheckCompletedCountInc();                                                                       // 已完成测试数加1
				// 新增：添加测试进度信息
				AppendInfo(QString("测试进度更新: %1/%2 组")
					.arg(DCR::DeviceCheckResultGlobal->GetCheckedGroupCount())
					.arg(TotalGroupCount));
				TASKWZ::TaskDataSend::SemaWaitForUI.release(1);                                                                               // 通知UI可以继续
				break;
			default:
				break;
			}
			break;
		case TASKWZ::TestStat::TEST_OVER:
		{
			WRITE_CENTRAL_WIDGET_DBG("TEST OVER\n");                                                                                          // 记录调试信息
			std::unique_lock lock(mtx);                                                                                                       // 加锁，保证线程安全
			ClockEvent();                                                                                                                     // 最后强制刷新一次
			Clock->stop();                                                                                                                    // 停止计时器
			if (TaskSend)
			{
				disconnect(TaskSend, &TASKWZ::TaskDataSend::MsgOfStartEnd, this, &CentralWidget::UpdateCheckResult);                          // 断开信号
				TaskSend->close();                                                                                                            // 关闭任务
				TaskSend = nullptr;                                                                                                           // 释放对象
			}
			TOOLWZ::stack_wz<TASKWZ::task_type> stack_end;                                                                                    // 创建任务类型栈
			stack_end.push(TASKWZ::TASK_DATA_SEND);                                                                                           // 压入"数据发送任务"
			CREATE_TASK_END(stack_end);                                                                                                       // 批量关闭任务
			StatOfBtnStart = true;                                                                                                            // 状态切换为"未测试"
			BtnStartTest.setText("开始测试");                                                                                                     // 按钮文本切换为"开始测试"

                                                                                                                                              // 生成带日期时间后缀的输出文件名
/* QString datetime = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
 QString processedLogPath = QString("./LOG/LogUpRecordProcessed_%1.log").arg(datetime);*/
			if (g_logOn) {                                                                                                                             // 结束时用同一个时间戳生成处理后日志名
				QString processedLogPath = QString("./LOG/LogUpRecordProcessed_%1.log").arg(testSessionTime);
				LPWZ::LogProcessor().processLog("./LOG/LogUpRecord.log", processedLogPath.toStdString());
				// 新增：添加测试完成信息
				AppendInfoWithTime("测试流程全部完成", "SUCCESS");
				AppendInfoWithTime(QString("处理后日志: %1").arg(processedLogPath), "INFO");
			}
			//                                                                                                                                // 调用日志处理
                                                                                                                                              //LPWZ::LogProcessor().processLog("./LOG/LogUpRecord.log", processedLogPath.toStdString());

			//LPWZ::LogProcessor().processLog("./LOG/LogUpRecord.log", "./LOG/LogUpRecordProcessed.log");                                     // 处理日志文件
			lock.unlock();                                                                                                                    // 解锁
		}
		break;
		default:
			break;
		}
		WRITE_CENTRAL_WIDGET_DBG("CentralWidget::UpdateCheckResult(), End\n");                                                                // 记录离开函数
	}

	void CentralWidget::UpdateVersion()
	{
		WRITE_CENTRAL_WIDGET_DBG("CentralWidget::UpdateVersion(), Enter\n");
		//顶部输出版本号
		VersionNum.setText(QString("20%1-%2-%3-%4-%5:R%6").arg(TASKWZ::TaskVersionParsing::Version.YEAR, 2, 16, QChar(u'0')).
			arg(TASKWZ::TaskVersionParsing::Version.MONTH, 2, 16, QChar(u'0')).arg(TASKWZ::TaskVersionParsing::Version.DAY, 2, 16, QChar(u'0')).
			arg(TASKWZ::TaskVersionParsing::Version.HOUR, 2, 16, QChar(u'0')).arg(TASKWZ::TaskVersionParsing::Version.MIN, 2, 16, QChar(u'0')).
			arg(TASKWZ::TaskVersionParsing::Version.VER, 5, 10, QChar(u'0')));

		//底部输出栏显示版本号和温度
		AppendInfoWithTime(QString("版本号: 20%1-%2-%3-%4-%5:R%6").arg(TASKWZ::TaskVersionParsing::Version.YEAR, 2, 16, QChar(u'0')).
			arg(TASKWZ::TaskVersionParsing::Version.MONTH, 2, 16, QChar(u'0')).arg(TASKWZ::TaskVersionParsing::Version.DAY, 2, 16, QChar(u'0')).
			arg(TASKWZ::TaskVersionParsing::Version.HOUR, 2, 16, QChar(u'0')).arg(TASKWZ::TaskVersionParsing::Version.MIN, 2, 16, QChar(u'0')).
			arg(TASKWZ::TaskVersionParsing::Version.VER, 5, 10, QChar(u'0')), "INFO");
		AppendInfoWithTime(QString("FPGA温度: %1℃, 环境温度:  %2℃").arg(DCR::DeviceCheckResultGlobal->GetTemperatureInner(), 4, 'f', 1, '0').arg(DCR::DeviceCheckResultGlobal->GetTempeartureEnv(), 4, 'f', 1, '0'), "INFO");
	}



	void CentralWidget::ClockEvent()
	{
                                                                                                                                              // 原有计时器逻辑
		PTestTime.setText(QString("%1:%2:%3")
			.arg(ClockVal / 3600, 2, 10, QChar('0'))
			.arg((ClockVal / 60) % 60, 2, 10, QChar('0'))
			.arg(ClockVal % 60, 2, 10, QChar('0')));
		ClockVal++;


		PTestedNum.setText(QString("%1(%2/%3)")                                                                                               // 新增：每秒刷新已测试次数/组数
			.arg(DCR::DeviceCheckResultGlobal->GetCheckCompletedCount())                                                                      // 传输已测试的次数
			.arg(DCR::DeviceCheckResultGlobal->GetCheckedGroupCount())                                                                        // 传输已测试的组值
			.arg(TotalGroupCount));                                                                                                           // 从文件中读取总的测试组数，只读取一次缓存值，不会每秒重复读取文件
	}

	void CentralWidget::NetConnected()
	{
		TimingDetection->start(1000);
		IfNoticeMNICWhenDisconnect = true;
		// 新增：添加网络连接信息
		AppendInfoWithTime("设备网络连接已建立", "SUCCESS");
		}

	void CentralWidget::NetDisconnected()
	{
		TimingDetection->stop();
		// 新增：添加网络断开信息
		AppendInfoWithTime("设备网络连接已断开", "ERROR");
		//UpdateChipOnlineStatus();                                                                                                           // 新增：定时刷新芯片在线状态
		ResetModelItemStat();
	}

	void CentralWidget::TimingDetectionEvent()
	{
		WRITE_CENTRAL_WIDGET_DBG("CentralWidget::TimingDetectionEvent(), Enter\n");
		UpdateChipOnlineStatus();                                                                                                             // 新增：每秒刷新一次在线状态和颜色
		TemperatureInner.setText(QString("%1℃").arg(DCR::DeviceCheckResultGlobal->GetTemperatureInner(), 4, 'f', 1, '0'));
		TemperatureEnv.setText(QString("%1℃").arg(DCR::DeviceCheckResultGlobal->GetTempeartureEnv(), 4, 'f', 1, '0'));
		
		// 检测网络连接状态
		if (DCR::DeviceCheckResultGlobal->GetUpPackCount() == UpPackCount)
		{
			// 上行包计数没有变化，可能断线了
			if (IfNoticeMNICWhenDisconnect)
			{
				IfNoticeMNICWhenDisconnect = false;
				ResetModelItemStat();
				AppendInfoWithTime("检测到网络连接中断", "WARNING");
				// 更新界面状态为断开
				UpdateConnectionStatus(false);
				emit NetLoss();
			}
		}
		else
		{
			// 上行包计数有变化，连接正常
			UpPackCount = DCR::DeviceCheckResultGlobal->GetUpPackCount();
			if (!IfNoticeMNICWhenDisconnect)
			{
				IfNoticeMNICWhenDisconnect = true;
				AppendInfoWithTime("网络连接已恢复", "SUCCESS");
				// 更新界面状态为已连接
				UpdateConnectionStatus(true);
				emit NetRecovery();
			}
		}
	}

	void CentralWidget::ResetModelItemStat()
	{
		for (int i = 0; i < 8; i++)
		{
			for (auto it : Models[i].Items)
			{
				it.RestOnLineStat();
			}
		}
	}

	/************************* 信息显示区域相关方法实现 *************************/
	
	void CentralWidget::InitInfoArea()
	{
		// 初始化信息显示文本区域
		InfoTextArea.setParent(this);
		InfoTextArea.setReadOnly(true);
		InfoTextArea.setMinimumHeight(160);   // 增大最小高度，显示更多行
		InfoTextArea.setMaximumHeight(240);   // 增大最大高度，显示更多行
		InfoTextArea.setPlaceholderText("测试信息将在此显示...");
		InfoTextArea.setStyleSheet(
			"QPlainTextEdit {"
			"    background-color: #f8f8f8;"
			"    border: 1px solid #d0d0d0;"
			"    border-radius: 4px;"
			"    font-family: 'Microsoft YaHei', 'Consolas';"
			"    font-size: 12px;"
			"    padding: 3px;"  // 减少内边距
			"}"
		);
		
		// 初始化控制按钮
		BtnClearInfo.setParent(this);
		BtnClearInfo.setText("清空");
		BtnClearInfo.setFixedSize(50, 20);  // 减小按钮尺寸
		BtnClearInfo.setToolTip("清空所有信息");
		
		BtnSaveInfo.setParent(this);
		BtnSaveInfo.setText("保存");
		BtnSaveInfo.setFixedSize(50, 20);   // 减小按钮尺寸
		BtnSaveInfo.setToolTip("保存信息到文件");
		
		// 初始化自动滚动复选框
		ChkAutoScroll.setParent(this);
		ChkAutoScroll.setText("自动滚动");
		ChkAutoScroll.setChecked(true);  // 默认开启自动滚动
		ChkAutoScroll.setToolTip("新信息添加时自动滚动到底部");
		
		// 设置控制按钮布局 - 将标题和控制按钮都放在一行
		HlInfoControls.addWidget(new QLabel("测试信息:", this));
		HlInfoControls.addStretch();
		HlInfoControls.addWidget(&ChkAutoScroll);
		HlInfoControls.addSpacing(5);  // 减少间距
		HlInfoControls.addWidget(&BtnClearInfo);
		HlInfoControls.addSpacing(3); // 减少间距
		HlInfoControls.addWidget(&BtnSaveInfo);
		HlInfoControls.setContentsMargins(3, 0, 3, 0);  // 减少边距
		
		// 设置信息区域整体布局
		VlInfoArea.addLayout(&HlInfoControls);
		VlInfoArea.addWidget(&InfoTextArea);
		VlInfoArea.setContentsMargins(3, 3, 3, 3);  // 减少边距
		VlInfoArea.setSpacing(3);  // 减少布局间距
		
		// 设置分组框
		GbInfoArea.setTitle("");  // 移除分组框标题，因为已在控制行显示
		GbInfoArea.setLayout(&VlInfoArea);
		GbInfoArea.setStyleSheet(
			"QGroupBox {"
			"    border: 1px solid #d0d0d0;"
			"    border-radius: 4px;"
			"    margin-top: 0px;"  // 减少上边距
			"    padding-top: 3px;" // 减少上内边距
			"}"
		);
		
		// 添加初始信息
		AppendInfoWithTime("系统初始化完成", "INFO");
	}
	
	void CentralWidget::AppendInfo(const QString& message, const QString& level)
	{
		// 根据级别设置不同颜色
		QString colorCode;
		if (level == "ERROR") {
			colorCode = "#ff4444";
		} else if (level == "WARNING") {
			colorCode = "#ff8800";
		} else if (level == "SUCCESS") {
			colorCode = "#00aa00";
		} else {
			colorCode = "#333333";  // INFO默认颜色
		}
		
		// 格式化消息
		QString formattedMessage = QString("<span style='color: %1'> %3</span>")
			.arg(colorCode)
			.arg(message);
		
		// 添加到文本区域
		InfoTextArea.appendHtml(formattedMessage);
		
		// 自动滚动到底部
		if (ChkAutoScroll.isChecked()) {
			QScrollBar* scrollBar = InfoTextArea.verticalScrollBar();
			scrollBar->setValue(scrollBar->maximum());
		}
		
		// 限制最大行数，防止内存占用过大，限制10000行
		QTextDocument* doc = InfoTextArea.document();
		if (doc->blockCount() > 10000) {
			QTextCursor cursor = InfoTextArea.textCursor();
			cursor.movePosition(QTextCursor::Start);
			cursor.select(QTextCursor::BlockUnderCursor);
			cursor.removeSelectedText();
		}
	}
	
	void CentralWidget::AppendInfoWithTime(const QString& message, const QString& level)
	{
		QString timeStamp = QDateTime::currentDateTime().toString("hh:mm:ss");
		QString messageWithTime = QString("[%1] %2").arg(timeStamp).arg(message);
		AppendInfo(messageWithTime, level);
	}
	
	void CentralWidget::OnClearInfo()
	{
		InfoTextArea.clear();
		AppendInfoWithTime("信息已清空", "INFO");
	}
	
	void CentralWidget::OnSaveInfo()
	{
		QString fileName = QFileDialog::getSaveFileName(
			this,
			"保存测试信息",
			QString("./LOG/TestInfo_%1.txt").arg(QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss")),
			"文本文件 (*.txt);;所有文件 (*.*)"
		);
		
		if (!fileName.isEmpty()) {
			QFile file(fileName);
			if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
				QTextStream out(&file);
				out << InfoTextArea.toPlainText();
				file.close();
				AppendInfoWithTime(QString("信息已保存到: %1").arg(fileName), "SUCCESS");
			} else {
				AppendInfoWithTime("保存失败: 无法创建文件", "ERROR");
			}
		}
	}
	
	void CentralWidget::OnAutoScrollToggled(bool enabled)
	{
		if (enabled) {
			AppendInfoWithTime("自动滚动已开启", "INFO");
			// 立即滚动到底部
			QScrollBar* scrollBar = InfoTextArea.verticalScrollBar();
			scrollBar->setValue(scrollBar->maximum());
		} else {
						AppendInfoWithTime("自动滚动已关闭", "INFO");
		}
	}
	
	// 新增：接收全局信息的槽函数实现
	void CentralWidget::OnGlobalInfoMessage(const QString& message, const QString& level)
	{
		AppendInfo(message, level);
	}

	/************************* 网络配置相关方法实现 *************************/
	
	void CentralWidget::InitNetworkConfig()
	{
		// 初始化IP地址验证器
		QString RangeIP = "(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])";
		QRegularExpression RegExpIP("^" + RangeIP + "\\." + RangeIP + "\\." + RangeIP + "\\." + RangeIP + "$");
		ValidatorIP = new QRegularExpressionValidator(RegExpIP, this);
		
		// 初始化端口验证器（1-65535）
		QRegularExpression RegExpPort("^([1-9][0-9]{0,3}|[1-5][0-9]{4}|6[0-4][0-9]{3}|65[0-4][0-9]{2}|655[0-2][0-9]|6553[0-5])$");
		ValidatorPort = new QRegularExpressionValidator(RegExpPort, this);
		
		// 设置验证器
		LeLocalIP.setValidator(ValidatorIP);
		LeDeviceIP.setValidator(ValidatorIP);
		LeLocalPort.setValidator(ValidatorPort);
		LeDevicePort.setValidator(ValidatorPort);
		
		// 设置占位符文本
		LeLocalIP.setPlaceholderText("192.168.1.100");
		LeLocalPort.setPlaceholderText("8254");
		LeDeviceIP.setPlaceholderText("192.168.1.200");
		LeDevicePort.setPlaceholderText("8204");
		
		// 设置按钮样式和状态
		BtnConnect.setText("连接");
		BtnConnect.setStyleSheet(
			"QPushButton {"
			"    background-color: #4CAF50;"
			"    color: white;"
			"    border: none;"
			"    border-radius: 3px;"
			"    font-weight: bold;"
			"}"
			"QPushButton:hover {"
			"    background-color: #45a049;"
			"}"
			"QPushButton:pressed {"
			"    background-color: #3d8b40;"
			"}"
			"QPushButton:disabled {"
			"    background-color: #cccccc;"
			"    color: #666666;"
			"}"
		);
		
		// 加载配置
		LoadNetworkConfig();
		
		// 初始化状态
		UpdateConnectionStatus(false);
	}
	
	void CentralWidget::LoadNetworkConfig()
	{
		// 从配置文件加载网络设置
		LeLocalIP.setText(CFGI::IniFileCFGGlobal->ReadINI(CFGI::INI_CENTRALIZE, "NETADDR/LocalIP").toString());
		LeLocalPort.setText(CFGI::IniFileCFGGlobal->ReadINI(CFGI::INI_CENTRALIZE, "NETADDR/LocalPort").toString());
		LeDeviceIP.setText(CFGI::IniFileCFGGlobal->ReadINI(CFGI::INI_CENTRALIZE, "NETADDR/DeviceIP").toString());
		LeDevicePort.setText(CFGI::IniFileCFGGlobal->ReadINI(CFGI::INI_CENTRALIZE, "NETADDR/DevicePort").toString());
		
		AppendInfoWithTime("网络配置已从文件加载", "INFO");
	}
	
	void CentralWidget::SaveNetworkConfig()
	{
		// 保存网络设置到配置文件
		CFGI::IniFileCFGGlobal->WriteINI(CFGI::INI_CENTRALIZE, "NETADDR/LocalIP", LeLocalIP.text());
		CFGI::IniFileCFGGlobal->WriteINI(CFGI::INI_CENTRALIZE, "NETADDR/LocalPort", LeLocalPort.text());
		CFGI::IniFileCFGGlobal->WriteINI(CFGI::INI_CENTRALIZE, "NETADDR/DeviceIP", LeDeviceIP.text());
		CFGI::IniFileCFGGlobal->WriteINI(CFGI::INI_CENTRALIZE, "NETADDR/DevicePort", LeDevicePort.text());
		
		AppendInfoWithTime("网络配置已保存到文件", "SUCCESS");
	}
	
	bool CentralWidget::ValidateNetworkInputs()
	{
		// 验证所有输入是否有效
		if (LeLocalIP.text().isEmpty() || !LeLocalIP.hasAcceptableInput()) {
			AppendInfoWithTime("本地IP地址格式不正确", "ERROR");
			return false;
		}
		
		if (LeLocalPort.text().isEmpty() || !LeLocalPort.hasAcceptableInput()) {
			AppendInfoWithTime("本地端口格式不正确", "ERROR");
			return false;
		}
		
		if (LeDeviceIP.text().isEmpty() || !LeDeviceIP.hasAcceptableInput()) {
			AppendInfoWithTime("设备IP地址格式不正确", "ERROR");
			return false;
		}
		
		if (LeDevicePort.text().isEmpty() || !LeDevicePort.hasAcceptableInput()) {
			AppendInfoWithTime("设备端口格式不正确", "ERROR");
			return false;
		}
		
		return true;
	}
	
	void CentralWidget::UpdateConnectionStatus(bool connected)
	{
		isConnected = connected;
		
		if (connected) {
			BtnConnect.setText("断开");
			BtnConnect.setStyleSheet(
				"QPushButton {"
				"    background-color: #f44336;"
				"    color: white;"
				"    border: none;"
				"    border-radius: 3px;"
				"    font-weight: bold;"
				"}"
				"QPushButton:hover {"
				"    background-color: #da190b;"
				"}"
				"QPushButton:pressed {"
				"    background-color: #b71c1c;"
				"}"
			);
			
			// 禁用输入框
			LeLocalIP.setEnabled(false);
			LeLocalPort.setEnabled(false);
			LeDeviceIP.setEnabled(false);
			LeDevicePort.setEnabled(false);
		} else {
			BtnConnect.setText("连接");
			BtnConnect.setStyleSheet(
				"QPushButton {"
				"    background-color: #4CAF50;"
				"    color: white;"
				"    border: none;"
				"    border-radius: 3px;"
				"    font-weight: bold;"
				"}"
				"QPushButton:hover {"
				"    background-color: #45a049;"
				"}"
				"QPushButton:pressed {"
				"    background-color: #3d8b40;"
				"}"
			);
			
			// 启用输入框
			LeLocalIP.setEnabled(true);
			LeLocalPort.setEnabled(true);
			LeDeviceIP.setEnabled(true);
			LeDevicePort.setEnabled(true);
		}
	}
	
	void CentralWidget::OnConnectBtnClicked()
	{
		if (!isConnected) {
			// 尝试连接
			if (!ValidateNetworkInputs()) {
				return;
			}
			
			AppendInfoWithTime(QString("尝试连接到 %1:%2").arg(LeDeviceIP.text()).arg(LeDevicePort.text()), "INFO");
			
			// 调用Socket连接
			if (SOCKWZ::SockGlob::Connect(LeLocalIP.text(), LeLocalPort.text(), 
										 LeDeviceIP.text(), LeDevicePort.text())) {
				// 连接成功
				SaveNetworkConfig();  // 保存配置
				UpdateConnectionStatus(true);
				AppendInfoWithTime("网络连接成功", "SUCCESS");
				
				// 直接调用NetConnected方法，启动定时检测
				NetConnected();
				
				// 同时发送信号通知其他组件
				emit NetRecovery();
			} else {
				// 连接失败
				AppendInfoWithTime("网络连接失败", "ERROR");
			}
		} else {
			// 断开连接
			AppendInfoWithTime("断开网络连接", "WARNING");
			SOCKWZ::SockGlob::DisConnect();
			UpdateConnectionStatus(false);
			
			// 直接调用NetDisconnected方法，停止定时检测
			NetDisconnected();
			
			// 同时发送信号通知其他组件
			emit NetLoss();
		}
	}
	
	void CentralWidget::OnNetworkInputChanged()
	{
		// 输入改变时验证按钮状态
		bool isValid = !LeLocalIP.text().isEmpty() && !LeLocalPort.text().isEmpty() &&
					   !LeDeviceIP.text().isEmpty() && !LeDevicePort.text().isEmpty();
		BtnConnect.setEnabled(isValid || isConnected);
	}

	/************************* 全局信息管理器实现 *************************/
	
	// 静态成员初始化
	InfoMessageManager* InfoMessageManager::instance = nullptr;
	
	InfoMessageManager* InfoMessageManager::getInstance()
	{
		if (instance == nullptr) {
			instance = new InfoMessageManager();
		}
		return instance;
	}
	
	void InfoMessageManager::postInfo(const QString& message, const QString& level)
	{
		InfoMessageManager* mgr = getInstance();
		emit mgr->infoMessageReady(message, level);
	}
	
	void InfoMessageManager::postInfoWithTime(const QString& message, const QString& level)
	{
		QString timeStamp = QDateTime::currentDateTime().toString("hh:mm:ss");
		QString messageWithTime = QString("[%1] %2").arg(timeStamp).arg(message);
		postInfo(messageWithTime, level);
	}
};

