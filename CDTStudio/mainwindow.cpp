#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "qgsscalecombobox.h"
#include "stable.h"

#include "cdtimagelayer.h"
#include "cdtextractionlayer.h"
#include "cdtsegmentationlayer.h"
#include "cdtclassificationlayer.h"
#include "cdtpixelchangelayer.h"
#include "cdtvectorchangelayer.h"

#include "cdtrecentfile.h"
#include "cdtprojecttabwidget.h"
#include "cdtprojectwidget.h"
#include "cdtattributedockwidget.h"
#include "cdtplot2ddockwidget.h"
#include "cdtdockwidget.h"
#include "cdttrainingsampledockwidget.h"
#include "cdtvalidationsampledockwidget.h"
#include "cdtcategorydockwidget.h"
#include "cdtextractiondockwidget.h"
#include "cdtprojecttreeitem.h"
#include "cdtundowidget.h"
#include "cdtlayerinfowidget.h"
//#include "cdttaskdockwidget.h"

#include "dialogabout.h"
#include "dialogconsole.h"

#ifdef Q_OS_WIN
#include "Windows.h"
#endif

MainWindow* MainWindow::mainWindow = Q_NULLPTR;
bool MainWindow::isLocked = false;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    recentFile(new CDTRecentFile("Project",this)),
    recentFileToolButton(new QToolButton(this))

{        
    ui->setupUi(this);
    mainWindow = this;

    initIconSize();
    initActions();
    initMenuBar();
    initToolBar();
    initDockWidgets();
    initStatusBar();    
    initConsole();

    recentFileToolButton->setText(tr("&Recent"));
    recentFileToolButton->setToolButtonStyle(ui->mainToolBar->toolButtonStyle());
    recentFileToolButton->setIcon(QIcon(":/Icons/RecentFiles.png"));
    recentFileToolButton->setPopupMode(QToolButton::InstantPopup);
    ui->mainToolBar->addWidget(recentFileToolButton);

    connect(ui->tabWidgetProject,SIGNAL(treeModelUpdated()),ui->treeViewObjects,SLOT(expandAll()));
    connect(ui->tabWidgetProject,SIGNAL(currentChanged(int)),this,SLOT(onCurrentTabChanged(int)));
    connect(ui->tabWidgetProject,SIGNAL(menuRecentChanged(QString)),recentFile,SLOT(addFile(QString)));
    connect(recentFile,SIGNAL(filesChanged(QStringList)),SLOT(updateRecentFiles(QStringList)));

//    connect(qApp,SIGNAL(taskInfoUpdated(QString ,int ,QString ,int ,int )),dockWidgetTask,SLOT(updateTaskInfo(QString,int,QString,int,int)));
//    connect(qApp,SIGNAL(taskCompleted(QString ,QByteArray )),dockWidgetTask,SLOT(onTaskCompleted(QString ,QByteArray)));

    QSettings setting("WHU","CDTStudio");
    this->restoreGeometry(setting.value("geometry").toByteArray());
    this->restoreState(setting.value("windowState").toByteArray());

    updateRecentFiles(recentFile->files());

    qDebug("MainWindow initialized");
}


MainWindow::~MainWindow()
{    
//    emit updateSetting();
    delete ui;
    qDebug("MainWindow destruct");
}

void MainWindow::initIconSize()
{
#if QT_VERSION < 0x050000
#ifdef Q_OS_WIN
    int dpiX = 96;
    int dpiY = 96;
    dpiX = GetDeviceCaps(this->getDC(),LOGPIXELSX);
    dpiY = GetDeviceCaps(this->getDC(),LOGPIXELSY);
    iconSize = QSize(dpiX*16/96,dpiY*16/96);
    ui->mainToolBar->setIconSize(iconSize);
#endif
#else
    ui->mainToolBar->setIconSize(QSize(24,24));
#endif


    qDebug("IconSize initialized");
}

void MainWindow::initActions()
{
    actionNew = new QAction(QIcon(":/Icons/New.png"),tr("&New"),this);
    actionNew->setShortcut(QKeySequence::New);
    actionNew->setStatusTip(tr("Create a new project"));
    connect(actionNew,SIGNAL(triggered()),SLOT(onActionNew()));

    actionOpen = new QAction(QIcon(":/Icons/Open.png"),tr("&Open"),this);
    actionOpen->setShortcut(QKeySequence::Open);
    actionOpen->setStatusTip(tr("Open one or more projects"));
    connect(actionOpen,SIGNAL(triggered()),SLOT(onActionOpen()));

    actionSave = new QAction(QIcon(":/Icons/Save.png"),tr("&Save"),this);
    actionSave->setShortcut(QKeySequence::Save);
    actionSave->setStatusTip(tr("Save current project"));
    connect(actionSave,SIGNAL(triggered()),SLOT(onActionSave()));

    actionSaveAll = new QAction(QIcon(":/Icons/SaveAll.png"),tr("Save A&ll"),this);
    actionSaveAll->setShortcut(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_S));
    actionSaveAll->setStatusTip(tr("Save all existing projects"));
    connect(actionSaveAll,SIGNAL(triggered()),SLOT(onActionSaveAll()));

    actionSaveAs = new QAction(QIcon(":/Icons/SaveAs.png"),tr("Save &As"),this);
    actionSaveAs->setShortcut(QKeySequence::SaveAs);
    actionSaveAs->setStatusTip(tr("Save current project as"));
    connect(actionSaveAs,SIGNAL(triggered()),SLOT(onActionSaveAs()));

    qDebug("Actions initialized");
}


void MainWindow::initMenuBar()
{
    QMenu* menuFile = new QMenu(tr("&File"),this);
    menuFile->addActions(QList<QAction*>()
                         <<actionNew
                         <<actionOpen
                         <<actionSave
                         <<actionSaveAll
                         <<actionSaveAs);
    menuFile->addSeparator();
    recentFileMenu = new QMenu(tr("&Recent"),this);
    recentFileMenu->setIcon(QIcon(":/Icons/RecentFiles.png"));
    menuFile->addMenu(recentFileMenu);

    QMenu* menuAbout = new QMenu(tr("&About"),this);
    menuAbout->addAction(tr("About &Qt"),qApp,SLOT(aboutQt()));
    menuAbout->addAction(tr("&About"),this,SLOT(about()));

    menuBar()->addMenu(menuFile);
    menuBar()->addMenu(menuAbout);
    qDebug("MenuBars initialized");
}

void MainWindow::initToolBar()
{
    ui->mainToolBar->addActions(QList<QAction*>()
                                <<actionNew
                                <<actionOpen
                                <<actionSave
                                <<actionSaveAll
                                <<actionSaveAs);
    qDebug("ToolBar initialized");
}

void MainWindow::initStatusBar()
{
    //Coordinates
    QLabel *labelCoord = new QLabel( QString(), statusBar() );
    labelCoord->setObjectName( "mCoordsLabel" );
    labelCoord->setMinimumWidth( 10 );
    labelCoord->setAlignment( Qt::AlignCenter );
    labelCoord->setFrameStyle( QFrame::NoFrame );
    labelCoord->setText( tr( "Coordinate:" ) );
    labelCoord->setToolTip( tr( "Current map coordinate" ) );
    statusBar()->addPermanentWidget( labelCoord, 0 );

    lineEditCoord = new QLineEdit( QString(), statusBar() );
    lineEditCoord->setObjectName( "lineEditCoord" );
    lineEditCoord->setMinimumWidth( 10 );
    lineEditCoord->setAlignment( Qt::AlignCenter );
    QRegExp coordValidator( "[+-]?\\d+\\.?\\d*\\s*,\\s*[+-]?\\d+\\.?\\d*" );
    new QRegExpValidator( coordValidator, lineEditCoord );
    lineEditCoord->setToolTip( tr( "Current map coordinate (lat,lon or east,north)" ) );
    statusBar()->addPermanentWidget( lineEditCoord, 0 );
    connect( lineEditCoord, SIGNAL( returnPressed() ), this, SLOT( userCenter() ) );

    // add a label to show current scale
    QLabel *scaleLabel = new QLabel( QString(), statusBar() );
    scaleLabel->setObjectName( "scaleLabel" );
    scaleLabel->setMinimumWidth( 10 );
    scaleLabel->setAlignment( Qt::AlignCenter );
    scaleLabel->setFrameStyle( QFrame::NoFrame );
    scaleLabel->setText( tr( "Scale:" ) );
    scaleLabel->setToolTip( tr( "Current map scale" ) );
    statusBar()->addPermanentWidget( scaleLabel, 0 );

    scaleEdit = new QgsScaleComboBox( statusBar() );
    scaleEdit->setObjectName( "scaleEdit" );
    scaleEdit->setMinimumWidth( 10 );
    scaleEdit->lineEdit()->setAlignment(Qt::AlignCenter);
    scaleEdit->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    scaleEdit->setToolTip( tr( "Current map scale (formatted as x:y)" ) );

    statusBar()->addPermanentWidget( scaleEdit, 0 );
    connect( scaleEdit, SIGNAL( scaleChanged(double) ), this, SLOT( userScale(double) ) );

    qDebug("StatusBar initialized");
}

void MainWindow::initDockWidgets()
{
    //Qt::RightDockWidgetArea
    dockWidgetTrainingSample = new CDTTrainingSampleDockWidget(this);
    dockWidgetTrainingSample->setObjectName("dockWidgetTrainingSample");
    registerDocks(Qt::RightDockWidgetArea,dockWidgetTrainingSample);

    dockWidgetValidationSample = new CDTValidationSampleDockWidget(this);
    dockWidgetValidationSample->setObjectName("dockWidgetValidationSample");
    registerDocks(Qt::RightDockWidgetArea,dockWidgetValidationSample);

    dockWidgetExtraction = new CDTExtractionDockWidget(this);
    registerDocks(Qt::RightDockWidgetArea,dockWidgetExtraction);

    //Qt::BottomDockWidgetArea
    dockWidgetCategory = new CDTCategoryDockWidget(this);
    dockWidgetCategory->setObjectName("dockWidgetCategory");
    registerDocks(Qt::BottomDockWidgetArea,dockWidgetCategory);

    dockWidgetAttributes = new CDTAttributeDockWidget(this);
    dockWidgetAttributes->setObjectName("dockWidgetAttributes");
    registerDocks(Qt::BottomDockWidgetArea,dockWidgetAttributes);

    dockWidgetPlot2D = new CDTPlot2DDockWidget(this);
    dockWidgetPlot2D->setObjectName("dockWidgetPlot2D");
    registerDocks(Qt::BottomDockWidgetArea,dockWidgetPlot2D);

    dockWidgetUndo = new CDTUndoWidget(this, Q_NULLPTR);
    registerDocks(Qt::BottomDockWidgetArea,dockWidgetUndo);

    //Qt::LeftDockWidgetArea
    dockWidgetLayerInfo = new CDTLayerInfoWidget(this);
    dockWidgetLayerInfo->setObjectName("dockWidgetLayerInfo");
    registerDocks(Qt::LeftDockWidgetArea,dockWidgetLayerInfo);

//    dockWidgetTask = new CDTTaskDockWidget(this);

    qDebug("Docks initialized");
}

void MainWindow::initConsole()
{
    DialogConsole* dialogConsole = new  DialogConsole(this);
    actionConsole = new QAction(tr("&Console"),this);
    actionConsole->setShortcut(QKeySequence(Qt::Key_F12));
    this->addAction(actionConsole);
    connect(actionConsole,SIGNAL(triggered()),dialogConsole,SLOT(show()));
    connect(actionConsole,SIGNAL(triggered()),dialogConsole,SLOT(updateDatabases()));

    qDebug("Console initialized");
}

void MainWindow::registerDocks(Qt::DockWidgetArea area,CDTDockWidget *dock)
{
    connect(ui->tabWidgetProject,SIGNAL(currentChanged(int)),dock,SLOT(onDockClear()));
    this->addDockWidget(area, dock);
    dock->raise();
    docks.push_back(dock);
}


MainWindow *MainWindow::getMainWindow()
{
    return mainWindow;
}

QTreeView *MainWindow::getProjectTreeView()
{
    return mainWindow->ui->treeViewObjects;
}

CDTTrainingSampleDockWidget *MainWindow::getTrainingSampleDockWidget()
{
    return mainWindow->dockWidgetTrainingSample;
}

CDTAttributeDockWidget *MainWindow::getAttributesDockWidget()
{
    return mainWindow->dockWidgetAttributes;
}

CDTPlot2DDockWidget *MainWindow::getPlot2DDockWidget()
{
    return mainWindow->dockWidgetPlot2D;
}

CDTExtractionDockWidget *MainWindow::getExtractionDockWidget()
{
    return mainWindow->dockWidgetExtraction;
}

CDTUndoWidget *MainWindow::getUndoWidget()
{
    return mainWindow->dockWidgetUndo;
}

CDTLayerInfoWidget *MainWindow::getLayerInfoWidget()
{
    return mainWindow->dockWidgetLayerInfo;
}

//CDTTaskDockWidget *MainWindow::getTaskDockWIdget()
//{
//    return mainWindow->dockWidgetTask;
//}

CDTProjectWidget *MainWindow::getCurrentProjectWidget()
{
    return (CDTProjectWidget *)(mainWindow->ui->tabWidgetProject->currentWidget());
}

QgsMapCanvas *MainWindow::getCurrentMapCanvas()
{
    CDTProjectWidget *projectWidget = getCurrentProjectWidget();
    if (projectWidget == Q_NULLPTR) return Q_NULLPTR;
    return projectWidget->mapCanvas;
}

QUuid MainWindow::getCurrentProjectID()
{
    CDTProjectWidget *prjWidget = getCurrentProjectWidget();
    if (prjWidget == 0)
        return QUuid();
    return prjWidget->project->id();
}

QSize MainWindow::getIconSize()
{
    return mainWindow->iconSize;
}

void MainWindow::onCurrentTabChanged(int i)
{
    if(i<0)
    {
        ui->treeViewObjects->setModel(Q_NULLPTR);
        lineEditCoord->setText(QString::null);
        scaleEdit->lineEdit()->setText(QString::null);
        return ;
    }

    CDTProjectWidget* projectWidget = (CDTProjectWidget*)(ui->tabWidgetProject->currentWidget());

    ui->treeViewObjects->setModel(projectWidget->treeModelObject);    
    ui->treeViewObjects->expandAll();
    ui->treeViewObjects->resizeColumnToContents(0);

    if (getCurrentMapCanvas())
    {
        getCurrentMapCanvas()->updateScale();
        showScale(getCurrentMapCanvas()->scale());
    }

    qDebug("Current tab is changed to %s",ui->tabWidgetProject->tabText(i));
}

void MainWindow::showMouseCoordinate(const QgsPointXY &p)
{
    if (this->getCurrentMapCanvas()==Q_NULLPTR)
        return;

    lineEditCoord->setText( QString("%1, %2").arg(p.x(), p.y()) );

    if ( lineEditCoord->width() > lineEditCoord->minimumWidth() )
    {
        lineEditCoord->setMinimumWidth( lineEditCoord->width() );
    }
}

void MainWindow::showScale(double theScale)
{
    scaleEdit->setScale( 1.0 / theScale );

    if ( scaleEdit->width() > scaleEdit->minimumWidth() )
    {
        scaleEdit->setMinimumWidth( scaleEdit->width() );
    }
}

void MainWindow::userCenter()
{
    QgsMapCanvas* mapCanvas = getCurrentMapCanvas();
    if (!mapCanvas)
        return;

    QStringList parts = lineEditCoord->text().split( ',' );
    if ( parts.size() != 2 )
        return;

    bool xOk;
    double x = parts.at( 0 ).toDouble( &xOk );
    if ( !xOk )
        return;

    bool yOk;
    double y = parts.at( 1 ).toDouble( &yOk );
    if ( !yOk )
        return;


    QgsRectangle r = mapCanvas->extent();

    mapCanvas->setExtent(
                QgsRectangle(
                    x - r.width() / 2.0,  y - r.height() / 2.0,
                    x + r.width() / 2.0, y + r.height() / 2.0
                    )
                );
    mapCanvas->refresh();
}

void MainWindow::userScale(double)
{
    QgsMapCanvas* mapCanvas = getCurrentMapCanvas();
    if (!mapCanvas)
        return;

    mapCanvas->zoomScale( 1.0 / scaleEdit->scale() );
}

void MainWindow::updateRecentFiles(QStringList list)
{
    //Clear all existing actions
    recentFileMenu->clear();
    while((recentFileToolButton->actions()).size()!=0)
        recentFileToolButton->removeAction(recentFileToolButton->actions()[0]);

    foreach (QString file, list) {
        QAction* recentFile = new QAction(file,this);
        recentFileMenu->addAction(recentFile);
        recentFileToolButton->addAction(recentFile);
        connect(recentFile,SIGNAL(triggered()),SLOT(onRecentFileTriggered()));
    }
}


void MainWindow::onActionNew()
{
    qDebug("Create a new project");
    ui->tabWidgetProject->createNewProject();
}

void MainWindow::onActionOpen()
{
    ui->tabWidgetProject->openProject();
}

void MainWindow::onActionSave()
{
    ui->tabWidgetProject->saveProject();
}

void MainWindow::onActionSaveAll()
{
    ui->tabWidgetProject->saveAllProject();
}

void MainWindow::onActionSaveAs()
{
    ui->tabWidgetProject->saveAsProject();
}

void MainWindow::onRecentFileTriggered()
{
    QAction* action = (QAction*)sender();
    ui->tabWidgetProject->openProject(action->text());
}

void MainWindow::about()
{
    DialogAbout *dlg = new DialogAbout(this);
    dlg->exec();
}

void MainWindow::on_treeViewObjects_customContextMenuRequested(const QPoint &pos)
{
    QModelIndex index =ui->treeViewObjects->indexAt(pos);
    CDTProjectWidget* curwidget =(CDTProjectWidget*) ui->tabWidgetProject->currentWidget();
    if(curwidget == Q_NULLPTR)
        return;

    CDTProjectTreeItem *item =static_cast<CDTProjectTreeItem *>(static_cast<QStandardItemModel *>(ui->treeViewObjects->model())->itemFromIndex(index));
    if(item == Q_NULLPTR)
        return;
    CDTBaseLayer* correspondingObject = item->correspondingObject();
    if (correspondingObject)
    {
        correspondingObject->onContextMenuRequest(this);
    }

    ui->treeViewObjects->expandAll();
}

void MainWindow::on_treeViewObjects_clicked(const QModelIndex &index)
{
    QStandardItemModel* model = (QStandardItemModel*)(ui->treeViewObjects->model());
    if (model == Q_NULLPTR)
        return;

    CDTProjectTreeItem *item =(CDTProjectTreeItem*)(model->itemFromIndex(index));
    if (item==Q_NULLPTR)
        return;

    if (item->correspondingObject() == Q_NULLPTR)
        return;

    foreach (CDTDockWidget *dock, docks) {
        dock->setCurrentLayer(item->correspondingObject());
    }

    int type = item->getType();
    if (type == CDTProjectTreeItem::EXTRACTION)
    {
        CDTExtractionLayer* extractionLayer = qobject_cast<CDTExtractionLayer*>(item->correspondingObject());
        if (extractionLayer != Q_NULLPTR)
        {
            extractionLayer->setOriginRenderer();
        }
    }
    else if (type == CDTProjectTreeItem::SEGMENTION)
    {
        CDTSegmentationLayer* segmentationLayer = qobject_cast<CDTSegmentationLayer*>(item->correspondingObject());
        if (segmentationLayer != Q_NULLPTR)
        {

            segmentationLayer->setOriginRenderer();
            if (segmentationLayer->canvasLayer()!=Q_NULLPTR)
            {
                getCurrentMapCanvas()->setCurrentLayer(segmentationLayer->canvasLayer());
                getCurrentMapCanvas()->refresh();
            }            
        }
    }
    else if (type == CDTProjectTreeItem::CLASSIFICATION)
    {
        CDTClassificationLayer* classificationLayer = qobject_cast<CDTClassificationLayer*>(item->correspondingObject());
        if (classificationLayer != Q_NULLPTR)
        {
            CDTSegmentationLayer* segmentationLayer = (CDTSegmentationLayer*)(classificationLayer->parent());
            QgsFeatureRenderer *renderer = classificationLayer->renderer();
            segmentationLayer->setRenderer(renderer);
            getCurrentMapCanvas()->refresh();
        }
    }    
    else if (type == CDTProjectTreeItem::PIXELCHANGE)
    {
        CDTPixelChangeLayer *layer = qobject_cast<CDTPixelChangeLayer *>(item->correspondingObject());
        if (layer != Q_NULLPTR)
        {
            getCurrentMapCanvas()->setCurrentLayer(layer->canvasLayer());
            getCurrentMapCanvas()->refresh();
        }

    }
    else if (type == CDTProjectTreeItem::VECTOR_CHANGE)
    {
        CDTVectorChangeLayer *layer = qobject_cast<CDTVectorChangeLayer *>(item->correspondingObject());
        if (layer != Q_NULLPTR)
        {
            layer->setOriginRenderer();
            getCurrentMapCanvas()->setCurrentLayer(layer->canvasLayer());
            getCurrentMapCanvas()->refresh();
        }
    }
}

//void MainWindow::updateTaskDock()
//{
//    QRect rect = this->geometry();
//    dockWidgetTask->resize(rect.width()/2,rect.height()/3);
//    dockWidgetTask->move(QPoint( rect.left()+rect.width()/2,rect.top()+rect.height()*2/3- ui->statusBar->height()));
//}

void MainWindow::clearAllDocks()
{
    foreach (CDTDockWidget *dock, docks) {
        dock->onDockClear();
    }
}

void MainWindow::moveEvent(QMoveEvent *e)
{
//    updateTaskDock();
    QMainWindow::moveEvent(e);
}

void MainWindow::resizeEvent(QResizeEvent *e)
{
//    updateTaskDock();
    QMainWindow::resizeEvent(e);
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    if (!ui->tabWidgetProject->closeAll())
    {
        e->ignore();
        return;
    }
    QSettings setting("WHU","CDTStudio");
    setting.setValue("geometry", saveGeometry());
    setting.setValue("windowState", saveState());
    qDebug("The state of Widgets has been saved!");
    QMainWindow::closeEvent(e);
}
