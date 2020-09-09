#include "MainWindow.h"
#include "ui_MainWindow.h"

//#include <pwd.h>
#include <QKeyEvent>

#include "TextDlg.h"
#include "ImageDlg.h"
#include "RenameDlg.h"

#include <QCheckBox>
#include <QStandardPaths>

#define MYDELETE(p)  {if (p) { delete   p; p = NULL;}}
#define MYDELETES(p) {if (p) { delete[] p; p = NULL;}}

#define QT_VERSION521

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //0. 初始化变量
    iniVar();

    //1. 初始化界面
    ui->setupUi(this);
    initUi();

    //2. 创建文件资源的模型
    createFileModel();

    //3. 分割主窗口
    createSplitter();

    //4. 创建左侧树形文件管理器
    createQTreeWidget();

    //5. 创建右侧的列表窗口视图
    createQTableView();

    //6. 创建右侧的图标窗口视图
    createQListView();
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::iniVar()
{
    m_pItem = NULL;
    m_pThread = NULL;
    m_pOldItem = NULL;
    m_pFileModel = NULL;
    m_iActionState = 0;
    m_pProgressBar = NULL;

    MYDELETE(m_pThread);
    m_pThread = new FileThread(this);
    connect(m_pThread, SIGNAL(copyAndCutFinished()), this, SLOT(onCopyAndCutFinished()));
    connect(m_pThread, SIGNAL(chooseCoverFileOrDir(bool, QString)), this, SLOT(onChooseCoverFileOrDir(bool, QString)));
    connect(this, SIGNAL(chooseCoverFinished(int)), m_pThread, SLOT(onChooseCoverFinished(int)));

}
void MainWindow::initUi()
{
    //选中列表按钮
    ui->tbList->setChecked(true);
    ui->tbList->setFocus();

    //隐藏图标按钮
    ui->tbIcon->setChecked(false);
}
void MainWindow::createFileModel()
{
    MYDELETE(m_pFileModel);
    m_pFileModel = new QFileSystemModel(this);
    m_pFileModel->setReadOnly(false);
    m_pFileModel->setFilter(QDir::AllEntries | QDir::NoDot);

    //默认排序  升序AscendingOrder, 降序DescendingOrder
    m_pFileModel->sort(0, Qt::AscendingOrder);
}
void MainWindow::createSplitter()
{
    //界面的比例分配
    ui->splitter->setStretchFactor(0, 20);       //左窗口占整个窗口的20%
    ui->splitter->setStretchFactor(1, 80);       //右窗口占整个窗口的80%
    ui->splitter->setAutoFillBackground(true);
}
void MainWindow::createQTreeWidget()
{
    //1. 设置列数 和 设置头的标题
    ui->tvFileList->setColumnCount(1);
    ui->tvFileList->setHeaderLabel(tr("目录结构"));

    //2. 默认加载系统桌面的所有文件夹和文件
    QString sDesktop = getDesktopPath();
    QDir dir(sDesktop);
    dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot); //不显示隐藏文件夹 和 文件

    if (dir.exists())
    {
        //树形Item对象，用于操作树形结构中的目录和文件
        MYDELETE(m_pItem);
        m_pItem = new QTreeWidgetItem(ui->tvFileList, QStringList(sDesktop));
        m_pItem->setIcon(0,QIcon(":/icon/icon_64_open.png"));    //设置图标
        m_pItem->setData(1, 0, sDesktop);

        m_items << m_pItem;

        //先读取桌面根目录下的所有目录，添加到树形窗口
        loadFiles(sDesktop, m_pItem);

        //再递归桌面的每个目录，把文件和文件夹都添加到树形窗口
        showChildrenCombox(m_pItem);

        //默认展开根目录
        if (m_pItem) m_pItem->setExpanded(true);
    }

    connect(ui->tvFileList, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(onItemClicked(QTreeWidgetItem*,int)));
}
void MainWindow::createQTableView()
{
    //1. 默认显示桌面下的所有目录和文件
    QString sDesktop = getDesktopPath();
    QDir dir(sDesktop);
    dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot); //不显示隐藏文件夹 和 文件

    if (dir.exists())
    {
        //显示S桌面所有目录 和 文件 到table
        ui->lvFileList->setModel(m_pFileModel);
        ui->lvFileList->setRootIndex(m_pFileModel->setRootPath(sDesktop));

        //展开所有节点
        if (m_pItem) m_pItem->setExpanded(true);

        //在界面显示桌面根目录
        m_sCurPath = sDesktop;
        ui->lePath->setText(sDesktop);
    }

    //2. 设置table的属性
    ui->lvFileList->setShowGrid(false);                                    //设置QTableView的透明化
    ui->lvFileList->setSelectionBehavior(QAbstractItemView::SelectRows);   //整行选中
    ui->lvFileList->setEditTriggers(QAbstractItemView::NoEditTriggers);    //不可编辑
    ui->lvFileList->verticalHeader()->hide();                              //隐藏行头

    //3. 设置列标题的排序功能
    QHeaderView *header = ui->lvFileList->horizontalHeader();
    header->setEnabled(true);

    //4. 自动适应内容
    ui->lvFileList->resizeColumnsToContents();
    ui->lvFileList->resizeRowsToContents();
    ui->lvFileList->horizontalHeader()->setStretchLastSection(true);

    //5. 设置列表项的宽度
    ui->lvFileList->setColumnWidth(0,200);
    ui->lvFileList->setColumnWidth(1,100);
    ui->lvFileList->setColumnWidth(2,100);

    //6. 连接点击信号槽
    connect(header, SIGNAL(sectionClicked(int)), this, SLOT(onSectionClicked(int)));
}
void MainWindow::createQListView()
{
    //1. 默认显示桌面下的所有目录和文件
    QString sDesktop = getDesktopPath();
    QDir dir(sDesktop);
    dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
    if (dir.exists())
    {
        ui->listView->setModel(m_pFileModel);
        ui->listView->setRootIndex(m_pFileModel->setRootPath(sDesktop));
        if (m_pItem) m_pItem->setExpanded(true);

        m_sCurPath = sDesktop;
        ui->lePath->setText(sDesktop);
    }

    ui->listView->setMinimumSize(QSize(0, 0));
    ui->listView->setMaximumSize(QSize(1870, 16777215));
    ui->listView->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->listView->setLayoutDirection(Qt::LeftToRight);

    ui->listView->setParent(0);
    ui->listView->hide();

    ui->listView->setViewMode(QListView::IconMode);
    ui->listView->setGridSize(QSize(120, 120));
    ui->listView->setIconSize(QSize(60, 60));
    ui->listView->setTextElideMode(Qt::ElideNone);

    //选择模式
    ui->listView->setSelectionMode(QAbstractItemView::SingleSelection);

    //文字可以换行显示
    ui->listView->setWordWrap(true);

    ui->listView->setMovement(QListView::Static);
    ui->listView->setResizeMode(QListView::Adjust);
}
void MainWindow::showChildrenCombox(QTreeWidgetItem* rootItem)
{
    int iSize = rootItem->childCount();
    if (iSize == 0) return;

    for (int i = 0; i < iSize; i++)
    {
        loadFiles(rootItem->child(i)->data(1, 0).toString(), rootItem->child(i));
    }
}
void MainWindow::loadFiles(QString sPath, QTreeWidgetItem *item)
{
    //1. 先判断SD是否存在
    QDir dir(sPath);
    if (!dir.exists()) return;

    //设置文件过滤器
    dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
    QFileInfoList list = dir.entryInfoList();

    int size = list.size();
    for (int i = 0; i < size; i++)
    {
        QFileInfo info = list.at(i);
        if (!info.isDir()) continue;

        //判断文件路径是否存在于QTreeWidgetItem之下， true为不存在
        if (!treeWidgetItemIsExisted(info.filePath(), item)) continue;

        QTreeWidgetItem *pFileItem = new QTreeWidgetItem(QStringList(info.fileName()), 0);  //0表示目录
        pFileItem->setIcon(0, QIcon(":/icon/icon_64_open.png"));
        pFileItem->setData(1, 0, info.filePath());
        item->addChild(pFileItem);

        m_items << pFileItem;
    }
}

bool MainWindow::treeWidgetItemIsExisted(QString sPath, QTreeWidgetItem* pParentItem)
{
    int nCount = pParentItem->childCount();
    if (nCount == 0) return true;

    for (int i = 0; i < nCount; i++)
    {
        if (pParentItem->child(i)->data(1, 0).toString() == sPath) return false;
    }

    return true;
}
void MainWindow::leftAndRightSync()
{
    ui->tvFileList->collapseAll();
    m_allPathList.clear();
    getAllLastPath(m_sCurPath);
    int nSize = m_allPathList.size();
    if (nSize == 0)
    {
        return;
    }
    QTreeWidgetItem* rootItem = NULL;
    if (m_pItem && (m_allPathList.at(nSize - 1) == m_pItem->data(1, 0).toString())) rootItem = m_pItem;

    if (rootItem) ui->tvFileList->expandItem(rootItem);

    for (int i = nSize - 2 ;i >= 0; i--)
    {
        rootItem = getChildItemByPath(rootItem, m_allPathList.at(i));
        if (rootItem) ui->tvFileList->expandItem(rootItem);
    }
}
QTreeWidgetItem* MainWindow::getChildItemByPath(QTreeWidgetItem* parent, QString sPath)
{
    QTreeWidgetItem* childItem = NULL;
    int nCount = parent->childCount();
    if (nCount == 0) return NULL;
    for (int i = 0; i < nCount; i++)
    {
        if (parent->child(i)->data(1, 0).toString() != sPath) continue;

        childItem = parent->child(i);
        break;
    }
    return childItem;
}
void MainWindow::getAllLastPath(QString sCurPath)
{
    if (sCurPath == "") return;
    if (sCurPath == getDesktopPath())
    {
        m_allPathList << sCurPath;
        return;
    }

    m_allPathList << sCurPath;
    QString str = sCurPath.mid(0, sCurPath.lastIndexOf("/"));
    getAllLastPath(str);
}
void MainWindow::prepareCopyAndCutPath()
{
    //1. 清空上一次复制 或 拷贝的文件路径
    m_sSourcePath.clear();

    //2. 判断当前是在列表模式 还是 图标模式
    QModelIndexList list = m_bViewState ? ui->lvFileList->selectedIndexes() : ui->listView->selectedIndexes();
    int nCount = list.size();

    int iStep = 1;
    if (m_bViewState)
    {
        #ifdef QT_VERSION521
        //在Qt5.2.1上， selectedIndexes()是
        //横向扫描文件的信息Name1, Size1, Type1, Date Modified1； Name2, Size2, Type2, Date Modified2；......
        iStep = 4;
        #else
        //在Qt4.8.6上， selectedIndexes()是
        //纵向扫描文件的信息Name1, Name2... ; Size1, Size2...; Type1, Type2...; Date Modified1, Date Modified2...
        nCount /= 4;
        #endif
    }
    else
    {
        iStep = 4;
    }

    //把用户选择的所有文件夹 和 文件路径保存起来，用于粘贴
    for (int i = 0; i < nCount; i += iStep)
    {
        m_sSourcePath << m_pFileModel->fileInfo(list.at(i)).filePath();
    }

    m_iActionState = -1;  //只要不等于0就可以
}
QString MainWindow::getPathName(QString sPath)
{
    if (!sPath.contains("/")) return sPath;
    return sPath.right(sPath.size() - sPath.lastIndexOf("/") - 1);
}
void MainWindow::freeListTreeWidgetItem()
{
    int iCount = m_items.count();
    for (int i = iCount - 1; i >= 0; i--)
    {
        QTreeWidgetItem* pItem = m_items[i];
        MYDELETE(pItem);
    }
    m_items.clear();
}
void MainWindow::updateTreeWidgetData()
{
    //0.先清空原来的目录结构
    freeListTreeWidgetItem();
    ui->tvFileList->clear();

    //1.添加SDCard目录
    QDir dir(getDesktopPath());
    dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
    if (dir.exists())
    {
        m_pItem = new QTreeWidgetItem(ui->tvFileList, QStringList(QString(getDesktopPath())));
        m_items << m_pItem;
        m_pItem->setIcon(0,QIcon(":/icon/icon_64_open.png"));
        m_pItem->setData(1, 0, QString(getDesktopPath()));

        loadFiles(getDesktopPath(), m_pItem);
        showChildrenCombox(m_pItem);
    }

    m_allPathList.clear();
    getAllLastPath(m_sCurPath);

    int nSize = m_allPathList.count();
    if (nSize >= 3)
    {
        QTreeWidgetItem* item = NULL;
        for (int j = 0; j < m_items.count(); j++)
        {
            if (m_items.at(j)->data(1, 0).toString() != m_allPathList.at(nSize - 3)) continue;

            item = m_items.at(j);
            break;
        }

        if (item) showChildrenCombox(item);
        for (int i = nSize - 4 ;i >= 0; i--)
        {
            if (!item) break;
            item = getChildItemByPath(item, m_allPathList.at(i));
            showChildrenCombox(item);
        }
    }
    m_pOldItem = NULL;        //确保槽函数中的的值同步

    leftAndRightSync();
}
void MainWindow::createDirOrFile(bool bFile)
{
    QString sPathDes = m_sCurPath + "/" + (bFile ? tr("新建文本文档0") : tr("新建文件夹0"));

    while (dirIsExistsCurrentPath(sPathDes, m_sCurPath))
    {
        int iNum = -1;
        QString sFileName = sPathDes.mid(sPathDes.lastIndexOf("/") + 1);

        int iLength = QString((bFile ? tr("无标题文档") : tr("NewFolder"))).length();

        if (bFile ? (sFileName.left(iLength) == tr("无标题文档")) : (sFileName.left(iLength) == tr("NewFolder")))
        {
            QString sNum = sFileName.mid(iLength);
            bool bNum = stringIsNumber(sNum);
            iNum = bNum ? sNum.toInt() : -1;
        }

        sPathDes = (iNum == -1) ? (m_sCurPath + "/" + (bFile ? tr("无标题文档%1").arg(1) : tr("NewFolder%1").arg(1)))
                                : (m_sCurPath + "/" + (bFile ? tr("无标题文档%1").arg(++iNum) : tr("NewFolder%1").arg(++iNum)));
    }

    if (bFile)
    {
        QFile file(sPathDes);
        file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text);
        file.close();
    }
    else
    {
        m_pFileModel->mkdir(m_pFileModel->index(m_sCurPath), sPathDes);
        updateTreeWidgetData();
    }
}
bool MainWindow::dirIsExistsCurrentPath(QString dirName, QString sCurPath)
{
    QDir dir(sCurPath);
    return dir.exists(dirName);
}
bool MainWindow::stringIsNumber(QString str)
{
    QByteArray ba = str.toLatin1(); //将QString转换成char*
    const char *s = ba.data();
    while(*s && *s >= '0' && *s <= '9') s++;
    return ((*s) == 0);
}
int MainWindow::dirOrFileConflict(bool bDir, QString sNameSrc)
{
    QMessageBox box(QMessageBox::Question
                    , tr(bDir ? "文件夹冲突" : "文件冲突")
                    , bDir ? (tr("替换文件夹\"") + sNameSrc + "\"?") : tr("文件已存在，是否覆盖？")
                    , QMessageBox::Cancel | QMessageBox::Ok
                    , this);

    box.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);

    box.setDefaultButton(QMessageBox::Ok);
    box.setButtonText(QMessageBox::Ok, bDir ? tr("替换") : tr("覆盖"));
    box.setButtonText(QMessageBox::Cancel, tr("取消"));

    QCheckBox* checkBox = new QCheckBox("应用到所有文件", &box);
    checkBox->setGeometry(60, 30, 100, 20);
    checkBox->show();
    int selectNum = box.exec();

    int iCoverType = checkBox->isChecked() ? 2 : 1;

    MYDELETE(checkBox);
    if (selectNum == QMessageBox::Cancel) return -1;

    return iCoverType;
}
void MainWindow::on_tbFind_clicked()
{
}
void MainWindow::on_tbList_clicked()
{
    //隐藏图标视图
    ui->listView->setParent(0);
    ui->listView->hide();

    //显示列表视图
    ui->lvFileList->setParent(ui->splitter);
    ui->lvFileList->show();

    //记录为列表视图
    m_bViewState = true;

    //开关状态
    ui->tbList->setChecked(true);
    ui->tbList->setFocus();

    ui->tbIcon->setChecked(false);
}
void MainWindow::on_tbIcon_clicked()
{
    //隐藏列表视图
    ui->lvFileList->setParent(0);
    ui->lvFileList->hide();

    //显示图标视图
    ui->listView->setParent(ui->splitter);
    ui->listView->show();

    //记录为列表视图
    m_bViewState = false;

    //界面的比例分配
    ui->splitter->setStretchFactor(0, 20);
    ui->splitter->setStretchFactor(1, 80);
    ui->splitter->setAutoFillBackground(true);

    //开关状态
    ui->tbIcon->setChecked(true);
    ui->tbIcon->setFocus();

    ui->tbList->setChecked(false);
}
void MainWindow::onItemClicked(QTreeWidgetItem* item, int /*i*/)
{
    showChildrenCombox(item);

    if (m_pOldItem) m_pOldItem->setIcon(0,QIcon(":/icon/icon_64_open.png"));
    m_pOldItem = item;

    m_sCurPath = item->data(1, 0).toString();
    item->setIcon(0,QIcon(":/icon/icon_64_liangpinxuexi.png"));

    //把当前路径显示lePath控件
    ui->lePath->setText(m_sCurPath);

    //点了哪一个item, 右边的ListView中出现所有文件夹 和 文件的信息
    //列表模式
    ui->lvFileList->setModel(m_pFileModel);
    ui->lvFileList->setRootIndex(m_pFileModel->setRootPath(m_sCurPath));

    //图标模式
    ui->listView->setModel(m_pFileModel);
    ui->listView->setRootIndex(m_pFileModel->setRootPath(m_sCurPath));
}
void MainWindow::onSectionClicked(int a)
{
    if (a < 0 || a > 3) return;
    static int a1 = 0;
    QHeaderView *header = ui->lvFileList->horizontalHeader();

    a1++;
    if (a1 == 1)
    {
        header->setSortIndicator(a, Qt::AscendingOrder);
        header->setSortIndicatorShown(true);
        m_pFileModel->sort(a, Qt::AscendingOrder);
    }
    else if (a1 == 2)
    {
        header->setSortIndicator(a, Qt::DescendingOrder);
        header->setSortIndicatorShown(true);
        m_pFileModel->sort(a, Qt::DescendingOrder);
        a1 = 0;
    }
}

QString MainWindow::getDesktopPath()
{
//    struct passwd *pwd = getpwuid(getuid());
//    return QString(pwd->pw_dir) + "/Desktop";
    return QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
}

void MainWindow::timerEvent(QTimerEvent */*event*/)
{
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
    //按 Esc 退出程序
    if (event->key() == Qt::Key_Escape)
    {
        qApp->exit(-1);
        return;
    }
}

void MainWindow::on_lvFileList_doubleClicked(const QModelIndex &index)
{
    //1. 取出文件信息或目录信息
    QFileInfo fi = m_pFileModel->fileInfo(index);

    //2. 判断是否是目录, 如果是目录，进入下一个目录
    if (fi.isDir())
    {
        if (fi.fileName() == ".." && m_sCurPath == getDesktopPath()) return;

        QModelIndex mIndex = m_pFileModel->index(fi.filePath());
        ui->lvFileList->setRootIndex(mIndex);
        m_sCurPath = m_pFileModel->filePath(mIndex);
        ui->lePath->setText(m_sCurPath);
    }

    //3. 左边树 和 右边点击同步展开
    leftAndRightSync();

    //4. 如果双击的是 文本文件 或者 图片文件， 则打开浏览
    if (fi.isFile())
    {
        if (!fi.exists() || fi.size() == 0) return;

        QString sFileName = fi.fileName();
        QString suff = sFileName.right(4);

        if (suff == ".bmp" || suff == ".png" || suff == ".yuv" || suff == ".jpg" || suff == "jpeg")
        {
            ImageDlg * pImageView = new ImageDlg(this);
            pImageView->show();
            pImageView->openImage(fi.filePath());
        }
        else if (suff == ".ini" || suff == ".bak" ||
                 suff == ".txt" || suff == ".cpp" ||
                 suff.right(2) == ".h"|| suff.right(3) == ".sh")
        {
            TextDlg * pFileViewer = new TextDlg(this, fi.filePath());
            pFileViewer->show();
        }
    }
}

void MainWindow::on_listView_doubleClicked(const QModelIndex &index)
{
    //1. 取出文件信息或目录信息
    QFileInfo fi = m_pFileModel->fileInfo(index);

    //2. 判断是否是目录, 如果是目录，进入下一个目录
    if (fi.isDir())
    {
        if (fi.fileName() == ".." && m_sCurPath == getDesktopPath()) return;

        bool bDotDot = (fi.fileName() == "..");
        ui->listView->setRootIndex(bDotDot ? index.parent().parent() : index);
        m_sCurPath = m_pFileModel->filePath(bDotDot ? index.parent().parent() : index);
        ui->lePath->setText(m_sCurPath);
    }

    m_pFileModel->sort(0);
    m_pFileModel->sort(1);

    //3. 左边树 和 右边点击同步展开
    leftAndRightSync();

    //4. 如果双击的是 文本文件 或者 图片文件， 则打开浏览
    if (fi.isFile())
    {
        QString sFileName = fi.fileName();
        QString suff = sFileName.right(4);

        if (suff == ".bmp" || suff == ".png" || suff == ".yuv" || suff == ".jpg" || suff == "jpeg")
        {
            ImageDlg * pImageView = new ImageDlg(this);
            pImageView->show();
            pImageView->openImage(fi.filePath());
        }
        else if (suff == ".ini" || suff == ".bak" ||
                 suff == ".txt" || suff == ".cpp" ||
                 suff.right(2) == ".h"|| suff.right(3) == ".sh")
        {
            TextDlg * pFileViewer = new TextDlg(this, fi.filePath());
            pFileViewer->show();
        }
    }
}

void MainWindow::on_lvFileList_clicked(const QModelIndex &index)
{
    //1. 取出文件信息或目录信息
    QFileInfo fi = m_pFileModel->fileInfo(index);

    //2. ..时不用显示
    if (fi.fileName() == "..") return;

    //3. 文件路径显示到界面
    ui->lePath->setText(fi.isDir() ? m_sCurPath : fi.filePath());
}

void MainWindow::on_listView_clicked(const QModelIndex &index)
{
    //1. 取出文件信息或目录信息
    QFileInfo fi = m_pFileModel->fileInfo(index);

    //2. ..时不用显示
    if (fi.fileName() == "..") return;

    //3. 文件路径显示到界面
    ui->lePath->setText(fi.isDir() ? m_sCurPath : fi.filePath());
}

void MainWindow::on_lvFileList_customContextMenuRequested(const QPoint &pos)
{
    customContextMenuRequested(pos, false);
}
void MainWindow::on_listView_customContextMenuRequested(const QPoint &pos)
{
    customContextMenuRequested(pos, true);
}

void MainWindow::customContextMenuRequested(const QPoint &pos, bool bListView)
{
    m_ptPress = pos;

    //开始
    QMenu menu0;
    QMenu menu1;//二级菜单

    //判断是在文件夹 还是 在文件上点击鼠标右键
    bool bValid = bListView ? ui->listView->indexAt(pos).isValid() : ui->lvFileList->indexAt(pos).isValid();

    if (bValid) //在文件夹 或 文件上点击鼠标右键
    {
        //判断当前在哪种模式 列表模式 还是 图标模式
        QModelIndex index =  bListView ? ui->listView->indexAt(pos) : ui->lvFileList->indexAt(pos);

        //如果是在 文件夹 上点击右键鼠标 则弹出打开
        if (m_pFileModel->isDir(index))
        {
            QAction* pActionOpen = menu0.addAction(tr("打开"));
            connect(pActionOpen, SIGNAL(triggered()), this, SLOT(onActionOpen()));
            menu0.addSeparator();
        }
        else//如果是在文本文件 和 图片文件 上点击鼠标右键 则创建二级菜单，让用户选择打开方式
        {
            QAction* pActionOpenWay = menu0.addAction(tr("打开方式"));

            //添加二级菜单 用图片浏览器打
            QAction* pActionOpenImage = menu1.addAction(tr("用图片浏览器打开"));
            connect(pActionOpenImage, SIGNAL(triggered()), this, SLOT(onActionOpenImage()));

            //添加二级菜单 用文本浏览器打开
            QAction* pActionOpenText = menu1.addAction(tr("用文本浏览器打开"));
            connect(pActionOpenText, SIGNAL(triggered()), this, SLOT(onActionOpenText()));

            pActionOpenWay->setMenu(&menu1);
        }

        QAction* pActionNewDir = menu0.addAction(tr("新建文件夹"));
        connect(pActionNewDir, SIGNAL(triggered()), this, SLOT(onActionNewDir()));

        QAction* pActionNewFile = menu0.addAction(tr("新建文档"));
        connect(pActionNewFile, SIGNAL(triggered()), this, SLOT(onActionNewFile()));

        menu0.addSeparator();

        QAction* pActionCopy = menu0.addAction(tr("复制"));
        connect(pActionCopy, SIGNAL(triggered()), this, SLOT(onActionCopy()));

        QAction* pActionCut = menu0.addAction(tr("剪切"));
        connect(pActionCut, SIGNAL(triggered()), this, SLOT(onActionCut()));

        QAction* pActionPaste = menu0.addAction(tr("粘贴"));
        connect(pActionPaste, SIGNAL(triggered()), this, SLOT(onActionPaste()));


        QAction* pActionRename = menu0.addAction(tr("重命名"));
        connect(pActionRename, SIGNAL(triggered()), this, SLOT(onActionRename()));

        QAction* pActionDelete = menu0.addAction(tr("删除"));
        connect(pActionDelete, SIGNAL(triggered()), this, SLOT(onActionDelete()));

        if (m_pFileModel->filePath(index).right(2) == "..")
        {
            pActionCopy->setEnabled(false);
            pActionCut->setEnabled(false);
            pActionRename->setEnabled(false);
            pActionDelete->setEnabled(false);
        }

        if (m_iActionState == 0) pActionPaste->setEnabled(false);
    }
    else //在空白处点击鼠标右键
    {
        QAction* pActionNewDir = menu0.addAction(tr("新建文件夹"));
        connect(pActionNewDir, SIGNAL(triggered()), this, SLOT(onActionNewDir()));

        QAction* pActionNewFile = menu0.addAction(tr("新建文档"));
        connect(pActionNewFile, SIGNAL(triggered()), this, SLOT(onActionNewFile()));

        menu0.addSeparator();

        QAction* pActionPaste = menu0.addAction(tr("粘贴"));
        connect(pActionPaste, SIGNAL(triggered()), this, SLOT(onActionPaste()));
        if (m_iActionState == 0) pActionPaste->setEnabled(false);
    }

    menu0.exec(QCursor::pos());
}
void MainWindow::onActionOpen()
{
    //1. 先判断当前是列表模式 还是 图标模式
    QModelIndex index =  m_bViewState ? ui->lvFileList->indexAt(m_ptPress) : ui->listView->indexAt(m_ptPress);

    //2. 取出文件信息
    QFileInfo fi = m_pFileModel->fileInfo(index);

    //3. 判断当前的文件是文件夹还是文件
    if (fi.isDir())
    {
        if (fi.fileName() == ".." && m_sCurPath == getDesktopPath()) return;

        QModelIndex mIndex = m_pFileModel->index(fi.filePath());
        m_bViewState ? ui->lvFileList->setRootIndex(mIndex) : ui->listView->setRootIndex(mIndex);

        m_sCurPath = m_pFileModel->filePath(mIndex);
        ui->lePath->setText(m_sCurPath);
    }

    if (!m_bViewState)
    {
        m_pFileModel->sort(0);
        m_pFileModel->sort(1);
    }

    leftAndRightSync();
}
void MainWindow::onActionCopy()
{
    m_iOperation = 0;
    prepareCopyAndCutPath();
}
void MainWindow::onActionCut()
{
    m_iOperation = 1;
    prepareCopyAndCutPath();
}
void MainWindow::onActionRename()
{
    QModelIndex index =  m_bViewState ? ui->lvFileList->indexAt(m_ptPress) : ui->listView->indexAt(m_ptPress);

    //设置可编辑
    m_bViewState ? ui->lvFileList->edit(index) : ui->listView->edit(index);

    //获取需要改名字的文件夹 或 文件的全路径
    QString sReNamePath = m_pFileModel->filePath(index);

    //通过路径
    QString sOldName = getPathName(sReNamePath);

    m_bViewState ?  ui->lvFileList->setCurrentIndex(m_pFileModel->index(sReNamePath))
                  : ui->listView->setCurrentIndex(m_pFileModel->index(sReNamePath));


    RenameDlg dlg(this);
    dlg.setRename(sOldName);
    dlg.exec();

    if (m_bViewState) ui->lvFileList->setCurrentIndex(m_pFileModel->index(sReNamePath));

    QString sNewNamePath = sReNamePath.left(sReNamePath.size() - sOldName.size()) + dlg.RenameInformation();
    if (m_pFileModel->isDir(m_pFileModel->index(sReNamePath)))
    {
        QDir dir(sReNamePath);
        dir.rename(sReNamePath, sNewNamePath);
    }
    else
    {
        QFile::rename(sReNamePath, sNewNamePath);
    }
    sReNamePath = sNewNamePath;

    m_bViewState ? ui->lvFileList->setCurrentIndex(m_pFileModel->index(sReNamePath))
                 : ui->listView->setCurrentIndex(m_pFileModel->index(sReNamePath));

    //重新更新目录
    updateTreeWidgetData();
}

//删除
void MainWindow::onActionDelete()
{
    QMessageBox box(QMessageBox::Question
                    , tr("温馨提示："), tr("删除后不可恢复，请慎重考虑！！！")
                    , QMessageBox::Cancel | QMessageBox::Ok
                    , this);
    box.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    box.setDefaultButton(QMessageBox::Ok);
    box.setButtonText(QMessageBox::Ok, tr("确认"));
    box.setButtonText(QMessageBox::Cancel, tr("取消"));
    int selectNum = box.exec();
    if (selectNum != QMessageBox::Ok) return;

    if (m_bViewState)
    {
        QModelIndexList list = ui->lvFileList->selectedIndexes();

        if (list.size() == 0) return;
        if (!list.at(0).isValid()) return;

        int nCount = list.size();
        for (int i = 0; i < nCount; i++)
        {
            QModelIndex index = list.at(i);
            if (m_pFileModel->filePath(index).right(2) == "..") continue;

            //删除文件夹/文件
            m_pFileModel->remove(index);
            if (m_pFileModel->isDir(index)) updateTreeWidgetData();
        }
    }
    else
    {
        QModelIndex index = ui->listView->indexAt(m_ptPress);
        if (!index.isValid()) return;

        //删除文件夹/文件
        m_pFileModel->remove(index);
        if (m_pFileModel->isDir(index)) updateTreeWidgetData();
    }
}
void MainWindow::onActionOpenImage()
{
    //获取鼠标点击的文件model下表索引
    QModelIndex index = m_bViewState ? ui->lvFileList->indexAt(m_ptPress) :
                                       ui->listView->indexAt(m_ptPress);

    //获取当前文件的文件信息
    QFileInfo fi = m_pFileModel->fileInfo(index);

    //判断当前文件信息是文件还是目录
    if (fi.isFile())
    {
        //创建图片浏览器将该文件打开
        ImageDlg * pImageDlg = new ImageDlg(this);
        pImageDlg->show();
        pImageDlg->openImage(fi.filePath());
    }
}
void MainWindow::onActionOpenText()
{
    //获取鼠标点击的文件model下表索引
    QModelIndex index = m_bViewState ? ui->lvFileList->indexAt(m_ptPress) :
                                       ui->listView->indexAt(m_ptPress);

    //获取当前文件的文件信息
    QFileInfo fi = m_pFileModel->fileInfo(index);

    //判断当前文件信息是文件还是目录
    if (fi.isFile())
    {
        TextDlg * pTextDlg = new TextDlg(this, fi.filePath());
        pTextDlg->show();
    }
}
void MainWindow::onActionNewDir()
{
    createDirOrFile(false);
}
void MainWindow::onActionNewFile()
{
    createDirOrFile(true);
}
void MainWindow::onActionPaste()
{
    MYDELETE(m_pProgressBar);
    m_pProgressBar = new QProgressBar(NULL);
    m_pProgressBar->setWindowTitle(tr("正在复制中，请稍候......"));
    m_pProgressBar->setWindowFlags(Qt::WindowStaysOnTopHint | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
    m_pProgressBar->move(700, 380);
    m_pProgressBar->resize(520, 40);
    m_pProgressBar->setRange(0, 0);  //范围设置为0到0，会产生动画效果
    m_pProgressBar->show();
    if (m_pThread->isRunning()) m_pThread->terminate();

    m_pThread->setCopyAndCutParam(m_sSourcePath, m_iOperation, m_pFileModel, m_sCurPath);
    m_pThread->start();
}
void MainWindow::onCopyAndCutFinished()
{
    m_pProgressBar->hide();
    m_pProgressBar->close();
    MYDELETE(m_pProgressBar);

    updateTreeWidgetData();
    if (m_iOperation == 1) m_iActionState = 0;
}
void MainWindow::onChooseCoverFileOrDir(bool bDir, QString sNameSrc)
{
    int bCover = dirOrFileConflict(bDir, sNameSrc);
    emit chooseCoverFinished(bCover);
}
