#include "vision_rx.h"
#include "vision_predict.h"
#include "string.h"
#include "control_def.h"


vsn_drive_t vsn_drive;

void vision_data_handler(uint8_t *Vision_Data) {
	static uint32_t cnt;
	static float yaw_last,pit_last;
    /* 视觉信息解包 */
    memcpy(vsn_drive.rx[NOW].buff, Vision_Data, VISION_RX_NUM);
    
    /* 异常检测 */
    if (vsn_drive.rx[NOW].data.eof != 0xCC&&vsn_drive.rx[NOW].data.eof!=0xdc) { /* 帧尾校验 */
        vsn_drive.status = TAIL_ERROR;
        return;
    } 
	else { /* 无异常情况 */
		cnt++;
		if(cnt %30 == 0){
			if(yaw_last == vsn_drive.rx[NOW].data.yaw && pit_last == vsn_drive.rx[NOW].data.pit){
				vision.stop_fire = 1;
			}else {
				vision.stop_fire = 0;
			}
			yaw_last = vsn_drive.rx[NOW].data.yaw;
			pit_last = vsn_drive.rx[NOW].data.pit;
		}
        
		vsn_drive.repeat_cnt = 0;
        vsn_drive.rx[LAST].data.cnt = vsn_drive.rx[NOW].data.cnt;
        if (vsn_drive.rx[NOW].data.aim_flag) {
            vsn_drive.status = AIMING;
        } else {
            vsn_drive.status = UNAIMING;
        }
    }
    
    /* NAN处理 */
    NAN_PROCESS(vsn_drive.rx[NOW].data.yaw, vsn_drive.rx[LAST].data.yaw);
    NAN_PROCESS(vsn_drive.rx[NOW].data.pit, vsn_drive.rx[LAST].data.pit);
    NAN_PROCESS(vsn_drive.rx[NOW].data.dis, vsn_drive.rx[LAST].data.dis);
    NAN_PROCESS(vsn_drive.rx[NOW].data.fire_flag, vsn_drive.rx[LAST].data.fire_flag);
    NAN_PROCESS(vsn_drive.rx[NOW].data.pos, vsn_drive.rx[LAST].data.pos);
    
    /* 视觉预测计算 */
    vsn_calc();
}
