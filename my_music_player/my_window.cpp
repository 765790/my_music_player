#include "my_window.h"
#include <qdebug.h>

MyWindow::MyWindow(const QSize& window_size) :m_windowsize(window_size)
{
	resize(m_windowsize);
	//qDebug() << "set window size" << m_windowsize;
	//主布局
	QHBoxLayout* mainlayout = new QHBoxLayout(this);

	QVBoxLayout* leftlayout = new QVBoxLayout();


	m_songlist = new QListWidget(this);

	leftlayout->addWidget(m_songlist);
	setLayout(leftlayout);

	m_playpause_button = new QPushButton("play", this);
	m_playpause_button->setFixedWidth(60);
	QHBoxLayout* button_layout = new QHBoxLayout();
	button_layout->addWidget(m_playpause_button);

	m_playnext_button = new QPushButton("next", this);
	m_playnext_button->setFixedWidth(60);
	m_progress_bar = new QSlider(Qt::Horizontal, this);
	m_progress_bar->setRange(0, 100);
	leftlayout->addWidget(m_progress_bar);
	button_layout->addWidget(m_playnext_button);

	leftlayout->addLayout(button_layout);

	//初始化音量滑块和音量显示
	m_volume_slider = new QSlider(Qt::Horizontal, this);
	m_volume_slider->setFixedWidth(80);
	m_volume_slider->setRange(0, 100);
	m_volume_slider->setValue(100);//初始值为100
	button_layout->addWidget(m_volume_slider);

	m_volume_label = new QLabel("100%", this);
	m_volume_label->setFixedWidth(40);
	m_volume_label->setAlignment(Qt::AlignCenter);
	button_layout->addWidget(m_volume_label);

	m_playqueue = new PlayQueue(this);

	//增加右侧显示播放列表的面板
	QWidget* right_panel = new QWidget();
	QVBoxLayout* rightlayout = new QVBoxLayout(right_panel);
	rightlayout->setContentsMargins(10, 0, 0, 0);//添加左边距

	QLabel* playtitle = new QLabel("播放列表",this);
	QPushButton* clear_button = new QPushButton("清空列表", this);
	clear_button->setFixedSize(80, 25);
	QHBoxLayout* title_layout = new QHBoxLayout();
	title_layout->addWidget(playtitle);
	title_layout->addStretch();
	title_layout->addWidget(clear_button);

	rightlayout->addLayout(title_layout);

	m_playlist_display = new QListWidget();
	rightlayout->addWidget(m_playlist_display);

	mainlayout->addLayout(leftlayout, 3);
	mainlayout->addWidget(right_panel, 1);

	connect(m_songlist, &QListWidget::doubleClicked, this, &MyWindow::onSongDoubleClicked);
	connect(m_playqueue, &PlayQueue::songChanged, this, &MyWindow::onSongChanged);
	connect(m_playpause_button, &QPushButton::clicked, this, &MyWindow::onPlayPauseButtonClicked);
	connect(m_playnext_button, &QPushButton::clicked, this, &MyWindow::onPlayNextButtonClicked);
	connect(m_playqueue, &PlayQueue::positionChanged, this, &MyWindow::updateProgress);
	connect(m_playqueue, &PlayQueue::durationChanged, this, &MyWindow::updateDuration);
	connect(m_progress_bar, &QSlider::sliderMoved, m_playqueue, &PlayQueue::setPosition);
	connect(m_volume_slider, &QSlider::sliderMoved, this, &MyWindow::onVolumeChanged);
	connect(m_playqueue, &PlayQueue::queueUpdated, this, &MyWindow::updateQueueDisplay);
	connect(clear_button, &QPushButton::clicked, this, &MyWindow::onClearPlayList);

	//设置支持列表菜单
	m_songlist->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(m_songlist, &QListWidget::customContextMenuRequested, [=](const QPoint& pos) {
		QContextMenuEvent event(QContextMenuEvent::Mouse, pos,m_songlist->mapToGlobal(pos));
		contextMenuEvent(&event);
		});

	loadSongList("d:\\song");
}

void MyWindow::setWindowName(const QString& name)
{
	setWindowTitle(name);
}

void MyWindow::loadSongList(const QString& filepath)
{
	m_folder = new QDir(filepath);

	QStringList filter;
	filter << "*.mp3";
	m_folder->setNameFilters(filter);

	QFileInfoList filelist = m_folder->entryInfoList();

	for (const QFileInfo& fileinfo : filelist) {
		m_songlist->addItem(fileinfo.fileName());
	}
}

void MyWindow::onSongDoubleClicked(const QModelIndex& index) {
	QListWidgetItem* song_item = m_songlist->item(index.row());
	QString firstsong_name = song_item->text();
	QUrl first_song = QUrl::fromLocalFile(m_folder->filePath(firstsong_name));

	QList<QUrl> song_list;
	for (int i = 0; i < m_songlist->count(); i++) {
		QString songname = m_songlist->item(i)->text();
		song_list.append(QUrl::fromLocalFile(m_folder->filePath(songname)));
	}

	m_playqueue->createQueue(song_list, first_song);
}


void MyWindow::onSongChanged(const QUrl& song) {
	if (song.isEmpty()) {
		m_progress_bar->setValue(0);
		m_playpause_button->setText("play");
	}
	else {
		qDebug() << "Now playing" << song.fileName();
		m_playpause_button->setText("pause");
	}
}

void MyWindow::onPlayPauseButtonClicked() {
	//暂停逻辑
	if (m_playqueue->isPlaying()) {
		m_playqueue->pause();
		m_playpause_button->setText("play");
	}
	else {
		m_playqueue->play();
		m_playpause_button->setText("pause");
	}
}

void MyWindow::contextMenuEvent(QContextMenuEvent* event)
{
	QPoint pos = event->pos();
	m_selected_song = m_songlist->itemAt(pos);
	if (m_selected_song) {
		QMenu menu;
		QAction* play_action = menu.addAction("播放");
		QAction* next_action = menu.addAction("下一首播放");

		connect(play_action, &QAction::triggered, this, &MyWindow::onSongMenuSelected);
		connect(next_action, &QAction::triggered, this, &MyWindow::onAddNextClicked);
		menu.exec(event->globalPos());
	}
}

void MyWindow::onPlayNextButtonClicked() {
	m_playqueue->playNext();
}

void MyWindow::updateProgress(qint64 position) {
	if (!m_progress_bar->isSliderDown()) {//检查进度条是否被用户按住
		m_progress_bar->setValue(position);
	}
}

void MyWindow::updateDuration(qint64 duration) {
	m_progress_bar->setRange(0, duration);
}

void MyWindow::onSongMenuSelected()
{
	if (!m_selected_song) return;

	QString selected_song_name = m_selected_song->text();
	QUrl selected_song_url = QUrl::fromLocalFile(m_folder->filePath(selected_song_name));

	QList<QUrl> song_list;
	for (int i = 0; i < m_songlist->count(); i++) {
		QString songname = m_songlist->item(i)->text();
		song_list.append(QUrl::fromLocalFile(m_folder->filePath(songname)));
	}

	m_playqueue->createQueue(song_list, selected_song_url);
}

void MyWindow::updateQueueDisplay(const QList<QUrl>& queue)
{
	m_playlist_display->clear();
	int index = 1;
	for (const QUrl& song_url : queue) {
		QFileInfo fileinfo(song_url.toLocalFile());
		m_playlist_display->addItem(QString("%1. %2").arg(index++).arg(fileinfo.fileName()));
	}
}

void MyWindow::onAddNextClicked()
{
	if (!m_selected_song) return;

	QString song_name = m_selected_song->text();
	QUrl song_url = QUrl::fromLocalFile(m_folder->filePath(song_name));

	m_playqueue->insertNextSong(song_url);
	if (m_playqueue->isPlaying()) {
		m_playqueue->play();
	}
}

void MyWindow::keyPressEvent(QKeyEvent* event)
{
	if (event->key() == Qt::Key_Escape) {
		close();
		qDebug() << "程序已退出！";
	}
	else {
		QWidget::keyPressEvent(event);
	}
}

void MyWindow::onVolumeChanged(int volume) {
	m_playqueue->setVolume(volume);
	m_volume_label->setText(QString::number(volume) + "%");
}

void MyWindow::onClearPlayList() {
	m_progress_bar->setValue(0);
	m_playpause_button->setText("play");
	m_playqueue->clearQueue();
}
