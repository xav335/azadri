#include <QtDebug>
#include <QFileDialog>
#include <QMessageBox>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "xmlhandler.h"
#include "dotfilegenerator.h"
#include "sourcefilegenerator.h"
#include "docfilegenerator.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

	createActions();
	createMenus();

	m_inFileHasBeenParsed = false;

	statusBar()->showMessage(tr("Ready"));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::open()
{
	//--------------------
	// Open XML file

	m_inFileName = QFileDialog::getOpenFileName(this, tr("Open AZ State Machine XML File"),
												QDir::currentPath(),
												tr("XAZSM Files (*.xazsm *.xml)"));
	if (m_inFileName.isEmpty())
		return;

	QFile inFile(m_inFileName);
	if (!inFile.open(QFile::ReadOnly | QFile::Text)) {
		QMessageBox::warning(this, tr("AZ State Machine"),
							 tr("Cannot read file %1:\n%2.")
							 .arg(m_inFileName)
							 .arg(inFile.errorString()));
		return;
	}

	m_inFileInfo.setFile( inFile );
	QDir::setCurrent( m_inFileInfo.absolutePath() );

	//--------------------
	// Parse opened file and save it in SMDescription

	m_stateMachineDesc.clear();

	XmlHandler handler(&m_stateMachineDesc);
	QXmlSimpleReader reader;
	reader.setContentHandler(&handler);

	QXmlInputSource xmlInputSource(&inFile);
	if( ! reader.parse(xmlInputSource) )
	{
		QMessageBox::warning( this, tr("AZ State Machine"),
							  tr("Cannot parse the file\n%1")
							  .arg( handler.errorString() ) );
	}
	else
	{
		m_inFileHasBeenParsed = true;
		ui->generateButton->setEnabled(true);
		statusBar()->showMessage(tr("XML file parsed"), 10000);
	}

	//m_stateMachineDesc.debugTransitionList();
}

void MainWindow::createActions()
{
	openAct = new QAction(tr("&Open..."), this);
	openAct->setShortcuts(QKeySequence::Open);
	connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

	exitAct = new QAction(tr("E&xit"), this);
	exitAct->setShortcuts(QKeySequence::Quit);
	connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));
}

void MainWindow::createMenus()
{
	fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(openAct);
	fileMenu->addAction(exitAct);

	menuBar()->addSeparator();
}

// Open/Create file in write mode
bool MainWindow::openOutFile( QFile & file )
{
	if( ! file.open(QFile::WriteOnly | QFile::Text) )
	{
		QMessageBox::warning( this, tr("AZ State Machine"),
							  tr("Cannot write file %1 :\n%2.")
							  .arg( file.fileName() )
							  .arg( file.errorString() ) );
		return false;
	}
	return true;
}

void MainWindow::on_generateButton_clicked()
{
	QString	_baseName;
//	QString	_dotFileName;
//	QString	_cFileName;
//	QString	_hFileName;
//	QString	_txtFileName;
	int _nbFilesGenerated = 0;

#if 0
	// Open a window to choose where to save the .dot file
	QString _dotFileName =
			QFileDialog::getSaveFileName(this, tr("Save State Machine Graphviz File"),
										 QDir::currentPath(),
										 tr("Graphviz Files (*.dot)"));
#endif

	if( ! m_inFileHasBeenParsed )
	{
		QMessageBox::warning(this, tr("Nothing to generate"),
							 tr("To be able to generate, you must open a XML State Machine Description file."));
		return;
	}

	// If there's a title, we use the formatedTitle (lowercase without space) to create files names
	if( ! m_stateMachineDesc.formatedTitle.isEmpty() )
	{
		_baseName = m_stateMachineDesc.formatedTitle;
	}
	else
	{
		// If there's no title, we use the XML file name (without extension)
		_baseName = m_inFileInfo.baseName();
	}

	// Generate .dot file
	if( ui->genDotCheckBox->isChecked() )
		if( generateDot( _baseName ) )
			_nbFilesGenerated++;

	// Generate .h file
	if( ui->genHCheckBox->isChecked() )
		if( generateH( _baseName ) )
			_nbFilesGenerated++;

	// Generate .c file
	if( ui->genCCheckBox->isChecked() )
		if( generateC( _baseName ) )
			_nbFilesGenerated++;

	// Generate .txt file
	if( ui->genTxtCheckBox->isChecked() )
		if( generateTxt( _baseName ) )
			_nbFilesGenerated++;


	statusBar()->showMessage(tr("%1 file(s) generated").arg(_nbFilesGenerated), 10000);
}

// Generate .dot file
bool MainWindow::generateDot( QString baseName )
{
	bool _ret = false;
	QString _filePath;

	_filePath = QDir::currentPath() + "/" + baseName + ".dot";
	QFile _file( _filePath );
	if( openOutFile( _file ) )
	{
		// Generate
		DotFileGenerator _generator( &_file );
		_generator.generate( m_stateMachineDesc );
		qDebug() << _filePath << " generated.";
		_ret = true;
	}
	return _ret;
}

// Generate .h file
bool MainWindow::generateH( QString baseName )
{
	bool _ret = false;
	QString _filePath;

	_filePath = QDir::currentPath() + "/" + baseName + ".h";
	QFile _file( _filePath );
	if( openOutFile( _file ) )
	{
		// Generate
		SourceFileGenerator _generator( &_file, m_stateMachineDesc );
		_generator.generateH();
		qDebug() << _filePath << " generated.";
		_ret = true;
	}
	return _ret;
}

// Generate .c file
bool MainWindow::generateC( QString baseName )
{
	bool _ret = false;
	QString _filePath;

	_filePath = QDir::currentPath() + "/" + baseName + ".c";
	QFile _file( _filePath );
	if( openOutFile( _file ) )
	{
		// Generate
		SourceFileGenerator _generator( &_file, m_stateMachineDesc );
		_generator.generateC();
		qDebug() << _filePath << " generated.";
		_ret = true;
	}
	return _ret;
}

// Generate .txt file
bool MainWindow::generateTxt( QString baseName )
{
	bool _ret = false;
	QString _filePath;

	_filePath = QDir::currentPath() + "/" + baseName + ".txt";
	QFile _file( _filePath );
	if( openOutFile( _file ) )
	{
		// Generate
		DocFileGenerator _generator( &_file );
		_generator.generate( m_stateMachineDesc );
		qDebug() << _filePath << " generated.";
		_ret = true;
	}
	return _ret;
}