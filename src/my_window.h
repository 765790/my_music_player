#pragma once
#ifndef MY_WINDOW_H
#define MY_WINDOW_H

#include <qwidget.h>
#include <qlistwidget.h>
#include <qlayout.h>
#include <qdir.h>
#include "PlayQueue.h"
#include <qpushbutton.h>
#include <qevent.h>
#include <qlabel.h>
#include <qmenu.h>

class MyWindow:public QWidget {
	Q_OBJECT;
public:
	MyWindow(const QSize& window_size);
	void setWindowName(const QString& name);

protected:
	void contextMenuEvent(QContextMenuEvent* event) override;

private slots:
	void onSongDoubleClicked(const QModelIndex& index);
	void onSongChanged(const QUrl& song);
	void onPlayPauseButtonClicked();
	void onPlayNextButtonClicked();
	void updateProgress(qint64 position);
	void updateDuration(qint64 duration);
	void onVolumeChanged(int volume);
	void onSongMenuSelected();
	void onAddNextClicked();
	void updateQueueDisplay(const QList<QUrl>& queue);
	void onClearPlayList();

private:
	QSize m_windowsize;
	QListWidget* m_songlist;
	QDir* m_folder;
	PlayQueue* m_playqueue;
	QPushButton* m_playpause_button;//控制暂停逻辑
	QPushButton* m_playnext_button;//播放下一首歌
	QSlider* m_progress_bar;//进度条
	QSlider* m_volume_slider;//音量控制滑块
	QLabel* m_volume_label;//音量控制显示
	QListWidgetItem* m_selected_song = nullptr;//右键选中的歌曲对象
	QListWidget* m_playlist_display;//播放列表显示

	void keyPressEvent(QKeyEvent* event) override;//重载处理按键函数
	void loadSongList(const QString& filepath);
};

#endif 