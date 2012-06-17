#include "SoundNode.h"

SoundNode::SoundNode(void)	
{
	Reset();
}

SoundNode::SoundNode(Sound* s) 
{
	Reset();
	SetSound(s);
}

void	SoundNode::Reset() 
{
	priority		= SOUNDPRIORTY_LOW;
	pitch			= 1.0f;
	volume			= 0.7f;
	radius			= 50.0f;
	timeLeft		= 0.0f;
	isLooping		= true;
	oalSource		= NULL;
	sound			= NULL;	
	//Part 2
	streamPos		= 0;
	isGlobal		= false;
	for(unsigned int i = 0; i < NUM_STREAM_BUFFERS; ++i) {
		streamBuffers[i] = 0;
	}
}

SoundNode::~SoundNode(void)	
{
	DetachSource();
}

bool		SoundNode::CompareNodesByPriority(SoundNode *a, SoundNode* b) 
{
	return (a->priority > b->priority) ? true : false;
}

void SoundNode::SetSound(Sound *s) 
{
	sound = s;
	DetachSource();
	if(sound)	
	{
		timeLeft = sound->GetLength();
		//Part 2:
		if(sound->IsStreaming())
		{
			alGenBuffers(NUM_STREAM_BUFFERS, streamBuffers);
		}
		else
		{
			alDeleteBuffers(NUM_STREAM_BUFFERS, streamBuffers);
		}
	}
}

void SoundNode::AttachSource(OALSource* s) 
{
	oalSource = s;

	if(!oalSource) {
		return;
	}

	oalSource->inUse = true;

	alSourceStop(oalSource->source);	
	alSourcef(oalSource->source, AL_MAX_DISTANCE, radius);
	alSourcef(oalSource->source, AL_REFERENCE_DISTANCE, radius * 0.2f);

	//if(timeLeft > 0) {
		if(sound->IsStreaming()) {	//Part 2
			streamPos = timeLeft;
			int numBuffered = 0;
			while(numBuffered < NUM_STREAM_BUFFERS) {
				double streamed = sound->StreamData(streamBuffers[numBuffered],streamPos);

				if(streamed) {
					streamPos -= streamed;
					++numBuffered;
				}
				else{
					break;
				}
			}
			alSourceQueueBuffers(oalSource->source, numBuffered, &streamBuffers[0]);
		}
		else{
			alSourcei(oalSource->source,AL_BUFFER,sound->GetBuffer());
			alSourcef(oalSource->source,AL_SEC_OFFSET,(sound->GetLength()/ 1000.0f) - (timeLeft / 1000.0f));

			//cout << "Attaching! Timeleft: " << (sound->GetLength()/ 1000.0) - (timeLeft / 1000.0) << endl;
			alSourcePlay(oalSource->source);
		}
		alSourcePlay(oalSource->source);
	//}
}

void SoundNode::DetachSource() 
{
	if(!oalSource) {
		return;
	}

	oalSource->inUse = false;

	alSourcef(oalSource->source,AL_GAIN,0.0f);
	alSourceStop(oalSource->source);
	alSourcei(oalSource->source,AL_BUFFER,0);
		
	if(sound && sound->IsStreaming()) 
	{	//Part 2
		int numProcessed = 0;
		ALuint tempBuffer;
		alGetSourcei( oalSource->source, AL_BUFFERS_PROCESSED, &numProcessed );
		while( numProcessed-- ) {
			alSourceUnqueueBuffers( oalSource->source, 1, &tempBuffer );
		} 
	}

	oalSource = NULL;
}

void SoundNode::UpdateSoundState(float msec)
{
	if(sound)
	{
		timeLeft -= (msec * pitch);
		
		if(isLooping)
		{
			while(timeLeft < 0)
			{
				timeLeft *= -1;
				//timeLeft += sound->GetLength();
				//streamPos += sound->GetLength();
			}
		}
		if(oalSource)
		{
			alSourcef(oalSource->source	,AL_GAIN	,volume);
			alSourcef(oalSource->source	,AL_PITCH	,pitch);
			alSourcef(oalSource->source, AL_MAX_DISTANCE, radius);
			alSourcef(oalSource->source, AL_REFERENCE_DISTANCE, radius * 0.2f);
			
			//Part 2
			Vector3 position;

			if(isGlobal)
			{
				position = SoundSystem::GetSoundSystem()->GetListener()->GetWorldTransform().GetPositionVector();
			}
			else{
				position = GetWorldTransform().GetPositionVector();
			}

			alSourcefv(oalSource->source,AL_POSITION,(float*)&position);

			if(sound->IsStreaming()) 
			{
				int numProcessed;
				alGetSourcei(oalSource->source, AL_BUFFERS_PROCESSED, &numProcessed);
				alSourcei(oalSource->source	,AL_LOOPING	,0);

				while(numProcessed--/* && streamPos > 0*/) {	//The && prevents clipping at the end of sounds!
					ALuint freeBuffer;

					alSourceUnqueueBuffers(oalSource->source, 1, &freeBuffer);

					streamPos -= sound->StreamData(freeBuffer,streamPos);
					alSourceQueueBuffers(oalSource->source, 1, &freeBuffer);

					if(streamPos < 0 && isLooping) {
						streamPos += sound->GetLength();
					}
				}
			}
			else{
				alSourcei(oalSource->source	,AL_LOOPING	,isLooping ? 1 : 0);
			}
		}
	}
}

void SoundNode::Update(float msec)	
{
	SoundSystem::GetSoundSystem()->AddSoundNode(this);
	SceneNode::Update(msec);
}