#include <ulog/ulog.h>
#include "JSSmartAudioImpl.h"


#define TAG "JSSmartAudio"
#define MINI_VOLUME 20

static aiot::SmtaPlayerStatus js_audio_state;
//c++直接读取底层数据，不应该读取该变量；多个函数用到，static定义放在外面。
static aui_play_time_t g_ptime;

static int js_audio_play_stop(){
    // resume lose efficacy
    if(smtaudio_stop(MEDIA_ALL) == 0){
        LOGD(TAG, "JS play stop success");   
        return 0;
    }else{
        LOGD(TAG, "JS play stop fail");  
        return -1;
    }
}

static int js_audio_play_pause(){
    smtaudio_state_t tempStatus=smtaudio_get_state_by_id(MEDIA_MUSIC);
    //按键暂停 vs  语音暂停
    if(tempStatus==SMTAUDIO_STATE_PLAYING){
        if(smtaudio_pause() == 0){
            LOGD(TAG, "JS play pause success");   
            return 0;
        }else{
            LOGD(TAG, "JS play pause fail");  
            return -1;
        }
    }
    return -1;
}

static int js_audio_play_resume(){
    if(smtaudio_resume() == 0){
        LOGD(TAG, "JS play resume success");   
        return 0;
    }else{
        LOGD(TAG, "JS play resume fail");  
        return -1;
    }
}
#if 0
static int js_audio_mute(){

    char stateBuf[20];
    const char* _mute = "true";

    if(smtaudio_get_state() != SMTAUDIO_STATE_MUTE){
        // 如果系统非静音，运行静音。
        if(!smtaudio_mute()){
            if(!smtaudio_pause()){
                LOGD(TAG, "smtaudio_pause mute success");
            }
            LOGD(TAG, "smtaudio mute success");
        }
        return 0;
    }else{
        LOGD(TAG, "smtaudio already mute !!!");
        return -1;
    }
}

static int js_audio_unmute(){

    char stateBuf[20];
    const char* _mute = "false";
    if(smtaudio_get_state() == SMTAUDIO_STATE_MUTE){
        // 如果系统静音，运行取消静音。
        smtaudio_mute();
        smtaudio_resume();
        LOGD(TAG, "smtaudio unmute success");
        return 0;
    }else{
        LOGD(TAG, "smtaudio already unmute !!!");
        return -1;
    }
}
#endif

static int js_audio_vol_get(){
    int cur_vol = smtaudio_vol_get();
    LOGD(TAG, "cur_vol is %d", cur_vol);
    return cur_vol;
}

static int js_audio_vol_set(int& vol){

    if(smtaudio_vol_set(vol) == 0){
        aos_kv_setint(VOLUME_SAVE_KV_NAME, vol);
        return 0;
    }else{
        LOGD(TAG, "JS vol_set fail");
        return -1;
    }
}

static int js_audio_set_speed(double speed){
    if(speed < 0.5 || speed > 1.5){
        LOGD(TAG, "input speed 0.5~2");
        return -1;
    }
    if(smtaudio_set_speed(speed) == 0){
        LOGD(TAG, "JS set speed success");
        return 0;
    }else{
        LOGD(TAG, "JS set speed fail"); 
        return -1;
    }
}

static int js_audio_get_speed(float *speed){

    if(smtaudio_get_speed(speed) == 0){
        LOGD(TAG, "JS get speed %lf",*speed);
        return 0;
    }else{
        LOGD(TAG, "JS get speed fail"); 
        return -1;
    }
}

static void js_audio_SmtaUrl(char **url){
    //获取 媒体播放地址
    //你好芯宝，杭州天气;此时进入音乐播放器，获取的url是ctrl_local_play.url
    //*url=smtaudio_get_play_url();
    smtaudio_state_t tempStatus=smtaudio_get_state_by_id(MEDIA_MUSIC);
    if(tempStatus==SMTAUDIO_STATE_PLAYING || tempStatus==SMTAUDIO_STATE_PAUSE || tempStatus==SMTAUDIO_STATE_STOP){
        *url=smtaudio_get_url_by_id(MEDIA_MUSIC);
    }
}

static void js_audio_SmtaDuration(int *duration){
    //获取 媒体播放地址
    //你好芯宝，杭州天气;此时进入音乐播放器，获取的url是ctrl_local_play.url
    //*url=smtaudio_get_play_url();
    smtaudio_state_t tempStatus=smtaudio_get_state_by_id(MEDIA_MUSIC);
    if(tempStatus==SMTAUDIO_STATE_PLAYING || tempStatus==SMTAUDIO_STATE_PAUSE || tempStatus==SMTAUDIO_STATE_STOP){
        *duration = smtaudio_get_duration_by_id(MEDIA_MUSIC);
    }
}

static void js_audio_get_status(){
    static smtaudio_state_t audio_state;
    //tts打断音乐时，音乐变成暂停；此时再按播放，预期可以打断tts并恢复播放
    //但实际是从头播放，而不是恢复播放
    //因为通过smtaudio_get_state()，获取的是ctrl_local_play.status。
    audio_state =smtaudio_get_state_by_id(MEDIA_MUSIC);
    switch (audio_state)
    {
    case SMTAUDIO_STATE_STOP:
        js_audio_state = aiot::STOPED;
        break;
    case SMTAUDIO_STATE_PLAYING:
        js_audio_state = aiot::PLAYING;
        break;
    case SMTAUDIO_STATE_PAUSE:
        js_audio_state = aiot::PAUSED;
        break;
    case SMTAUDIO_STATE_MUTE:
        js_audio_state = aiot::MUTE;
        break;
    default:
        js_audio_state = aiot::UNKNOWN;
        break;
    }
    LOGD(TAG, "js cur audio status: %d", js_audio_state);
}

static void js_audio_seek_time(int seek_time_ms){
    //tts打断music时，虽然tts结束后会恢复music播放;但如果在恢复之前按快进，通过smtaudio_get_play_url()获取的url则是tts的url了
    char *url=smtaudio_get_url_by_id(MEDIA_MUSIC);
    static char *tempUrl=NULL;
 
    if(url && url[0]!='\0'){
        //必须分配内存，因为底层的url可能被释放
        if (tempUrl) {
            free(tempUrl);
        }
        tempUrl = strdup(url);
        LOGD(TAG, "js seek play: %s %d", tempUrl, seek_time_ms);
        if(smtaudio_start(MEDIA_MUSIC, tempUrl, seek_time_ms, 1) != 0){
            LOGE(TAG, "js seek play: failed");
        }
    }
    else{
        LOGE(TAG, "js seek play:get play url error");
    }  
}

namespace aiot
{
void JSSmartAudioImpl::init(JQuick::sp<JQPublishObject> pub)
{
    _pub = pub;

    // 默认取消静音
    if(smtaudio_get_state() == SMTAUDIO_STATE_MUTE){
        smtaudio_mute();
    }
}

int JSSmartAudioImpl::play(std::string url, int type, int resume){ //TODO
    if(type==MEDIA_MUSIC && smtaudio_get_state_by_id(MEDIA_SYSTEM)==SMTAUDIO_STATE_PLAYING){
        smtaudio_stop(MEDIA_SYSTEM);
    }

    if(smtaudio_start(type, const_cast<char *>(url.c_str()), 0, resume) == 0){
        LOGD(TAG, "JS play %s media %s success",type ? "system":"music", url.c_str());   
        return 0;
    }else{
        LOGD(TAG, "JS play %s media %s fail",type ? "system":"music",url.c_str());  
        return -1;
    }
}

int JSSmartAudioImpl::stop(void){ //TODO finish
    if(!js_audio_play_stop()){
        return 0;
    }else{
        LOGD(TAG,  "%s",__FUNCTION__ );
        return -1;
    }
    
}

int JSSmartAudioImpl::pause(void){    //TODO finish
    if(!js_audio_play_pause()){
        return 0;
    }else{
        LOGD(TAG,  "%s",__FUNCTION__ );
        return -1;
    }
}

int JSSmartAudioImpl::resume(void){    //TODO finish
    if(smtaudio_get_state_by_id(MEDIA_SYSTEM)==SMTAUDIO_STATE_PLAYING){
        smtaudio_stop(MEDIA_SYSTEM);
    }
    if(!js_audio_play_resume()){
        return 0;
    }else{
        LOGD(TAG,  "%s",__FUNCTION__ );
        return -1;
    }
}

//按键暂停时：smtaudio_resume_list_head指向一个中断原因为SMTAUDIO_INTERRUPT_REASON_BY_USER的节点
//只能通过JSSmartAudio::resume手动取出暂停节点，然后调用节点的恢复函数

//高优先级的音源打断音乐播放时：smtaudio_resume_list_head指向一个中断原因为MEDIA_SYSTEM的节点
//在smtaudio_event_task中检测到高优先级的MEDIA_SYSTEM播放完成后，自动取出中断原因为MEDIA_SYSTEM的节点，然后调用节点的恢复函数

//resumeEnable控制恢复节点的有效性，enable=0时，无论自动场景还是手动场景，都无法取出有效的恢复节点，因此也就无法恢复被暂停的音乐了。
int JSSmartAudioImpl::resumeEnable(int enable){    //TODO finish
    smtaudio_enable_ready_list(enable);
    return 1;
}

int JSSmartAudioImpl::mute(void){
    if (smtaudio_get_state() == SMTAUDIO_STATE_MUTE) {
        return -1;
    }

    smtaudio_mute();
    
    if (smtaudio_get_state() == SMTAUDIO_STATE_MUTE) {
        return 0;
    }

    return -1;
}

int JSSmartAudioImpl::unmute(void){
    if (smtaudio_get_state() != SMTAUDIO_STATE_MUTE) {
        return -1;
    }

    smtaudio_unmute();
    
    if (smtaudio_get_state() != SMTAUDIO_STATE_MUTE) {
        return 0;
    }

    return -1;
}

int JSSmartAudioImpl::getVol(void){ //TODO finish
    return js_audio_vol_get();
}

int JSSmartAudioImpl::setVol(int vol){ //TODO finish
    // smtaudio_vol_set(vol);
    if(js_audio_vol_set(vol)){
        return 0;
    }else{
        LOGD(TAG,  "%s",__FUNCTION__ );
        return -1;
    }
}

int JSSmartAudioImpl::seek(int seek_time_ms)
{
    //TODO 指定到快进时间位置 aui_player_seek_play
    if(smtaudio_get_state_by_id(MEDIA_SYSTEM)==SMTAUDIO_STATE_PLAYING){
        smtaudio_stop(MEDIA_SYSTEM);
    }
    js_audio_seek_time(seek_time_ms);
    return 0;
}

int JSSmartAudioImpl::setSpeed(double speed){ //TODO finish
    if(!js_audio_set_speed(speed)){
        return 0;
    }else{
        LOGD(TAG,  "%s",__FUNCTION__ );
        return -1;
    }
}

float JSSmartAudioImpl::getSpeed(void)
{
    //TODO 
    float speed;
    if(!js_audio_get_speed(&speed)){
        LOGD(TAG, "JS get speed %f",speed);
        return speed;
    }else{
        LOGD(TAG,  "%s",__FUNCTION__ );
        return -1;
    }
}

SmtaPlayerStatus JSSmartAudioImpl::getStatus(void){ //TODO finish
    js_audio_get_status();
    return js_audio_state;
}

int JSSmartAudioImpl::getCurTime(void){ 
    //TODO finish
    smtaudio_get_time(&g_ptime);
    //printf("get current time !!!! %lld \t %lld!!!! \n\n", g_ptime.curtime, g_ptime.duration);
    return g_ptime.curtime;
}


void JSSmartAudioImpl::getUrl(char **url)
{
    //TODO
    js_audio_SmtaUrl(url);
}

void JSSmartAudioImpl::getDuration(int *duration)
{
    //TODO
    js_audio_SmtaDuration(duration);
}

}  // namespace aiot
