#pragma once
#ifndef PLAY_QUEUE_H
#define PLAY_QUEUE_H
#include <qobject.h>
#include <qlist.h>
#include <qurl.h>
#include <qmediaplayer.h>
#include <qaudiooutput.h>
#include <random>

class PlayQueue :public QObject {
	Q_OBJECT;
public:
	explicit PlayQueue(QObject* parent = nullptr);
	void createQueue(const QList<QUrl>& song_list, const QUrl& first_song);//创建播放队列
	void playNext();//播放下一首
	void shuffleSongList();//随机打乱播放队列
	bool isPlaying() const;
	void pause();
	void play();
	void setVolume(int volume);
	void insertNextSong(const QUrl& song);//下一首播放功能

	QUrl getCurrentSong();
	int getCurrentIndex();

signals:
	void songChanged(const QUrl& song);//歌曲切换时发送信号
	void positionChanged(qint64 position);//进度条位置更新时发送信号
	void durationChanged(qint64 duration);//进度条总长度更新时发送信号
	void queueUpdated(const QList<QUrl>& m_queue);//播放队列更新时发送

public slots:
	void setPosition(qint64 position);//允许用户手动更新进度条
	void clearQueue();//清空播放列表

private slots:
	void handleMediaStatusChange(QMediaPlayer::MediaStatus status);


private:
	QList<QUrl> m_queue;//播放队列
	int m_current_index;
	QMediaPlayer* m_mediaplayer;
	QAudioOutput* m_audio;
};

#endif // !PLAY_QUEUE_H
