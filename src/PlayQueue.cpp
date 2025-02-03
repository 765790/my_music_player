#include "PlayQueue.h"

PlayQueue::PlayQueue(QObject* parent)
	:QObject(parent),m_current_index(-1)
{
	m_mediaplayer = new QMediaPlayer(this);
	m_audio = new QAudioOutput(this);
	m_mediaplayer->setAudioOutput(m_audio);

	connect(m_mediaplayer, &QMediaPlayer::mediaStatusChanged, 
		this, &PlayQueue::handleMediaStatusChange);
	connect(m_mediaplayer, &QMediaPlayer::positionChanged, this, &PlayQueue::positionChanged);
	connect(m_mediaplayer, &QMediaPlayer::durationChanged, this, &PlayQueue::durationChanged);
}

void PlayQueue::createQueue(const QList<QUrl>& song_list, const QUrl& first_song)
{
	m_queue = song_list;
	m_current_index = m_queue.indexOf(first_song);

	if (m_current_index == -1) {
		qWarning() << "first song does not exist!";
		return;
	}
	//将first_song移动到队列头
	m_queue.move(m_current_index, 0);
	m_current_index = 0;

	shuffleSongList();//打乱队列

	if (!m_queue.isEmpty()) {
		m_mediaplayer->setSource(m_queue.first());
		m_mediaplayer->play();

		emit songChanged(m_queue.first());
		emit queueUpdated(m_queue);
	}
}

void PlayQueue::playNext()
{
	m_current_index++;
	//检查索引是否合法
	if (m_current_index >= m_queue.size()) return;

	m_mediaplayer->setSource(m_queue.at(m_current_index));
	m_mediaplayer->play();

	emit songChanged(m_queue.at(m_current_index));
}

void PlayQueue::shuffleSongList()
{
	if (m_queue.size() > 1) {
		std::random_device rd;
		std::mt19937 g(rd());
		std::shuffle(m_queue.begin() + 1, m_queue.end(), g);
	}
}

bool PlayQueue::isPlaying() const
{
	return (m_mediaplayer->playbackState() == QMediaPlayer::PlayingState);
}

void PlayQueue::pause()
{
	m_mediaplayer->pause();
}

void PlayQueue::play()
{
	m_mediaplayer->play();
}

void PlayQueue::setVolume(int volume)
{
	m_audio->setVolume(volume / 100.0f);
}

void PlayQueue::insertNextSong(const QUrl& song)
{
	//检查是否存在播放队列
	if (getCurrentIndex() == -1) {
		createQueue({ song }, song);
	}
	else {
		m_queue.insert(getCurrentIndex() + 1, song);
		emit queueUpdated(m_queue);
	}
}

QUrl PlayQueue::getCurrentSong()
{
	return m_queue.at(m_current_index);
}

int PlayQueue::getCurrentIndex()
{
	return m_current_index;
}

void PlayQueue::handleMediaStatusChange(QMediaPlayer::MediaStatus status) {
	if (status == QMediaPlayer::EndOfMedia) {//结束则播放下一首歌
		playNext();
	}
}

void PlayQueue::setPosition(qint64 position) {
	m_mediaplayer->setPosition(position);
}

void PlayQueue::clearQueue() {
	m_queue.clear();
	m_current_index = -1;
	m_mediaplayer->stop();

	emit queueUpdated(m_queue);
	emit songChanged(QUrl());//发送空歌曲表示清空列表
}