#include "image.h"
#include "Ring.h"

volatile uint8 out_track_flag = 0;   // 1 表示小车已出界，锁死停车

// 图像巡线处理
static const uint8 straight_weight_array[SEARCH_IMAGE_H] = {
	1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,
	19,19,19,19,19,19,19,19,19,19,
	19,19,19,19,19,19,19,19,19,19,
	19,19,19,19,19,19,19,19,19,19,
	19,19,19,19,19,19,19,19,19,19,
	6,6,6,6,6,6,6,6,6,6,
	6,6,6,6,6,6,6,6,6,6,
	6,6,6,6,6,6,6,6,6,6,
	19,19,19,19,19,19,19,19,19,19
};
static const uint8 curve_weight_array[SEARCH_IMAGE_H] = {
    1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,
	10,10,10,10,10,10,10,10,10,10,
    10,10,10,10,10,10,10,10,10,10,
	20,20,20,20,20,20,20,20,20,20,
    20,20,20,20,20,20,20,20,20,20,
    20,20,20,20,20,20,20,20,20,20,
    20,20,20,20,20,20,20,20,20,20
};
static const uint8 ring_weight_array[SEARCH_IMAGE_H] = {
    1,1,1,1,1,1,1,1,1,1,
    2,2,2,2,2,3,3,3,3,3,
    4,4,5,5,6,7,8,9,10,10,
    10,10,10,10,10,10,10,10,10,10,
    13,13,13,13,13,13,13,13,13,13,
    16,16,16,16,16,16,16,16,16,16,
    17,17,17,17,17,17,17,17,17,17,
    18,18,18,18,18,18,18,18,18,18,
    18,18,18,18,18,18,18,18,18,18,
    17,17,17,17,17,17,17,17,17,17,
    13,13,13,13,13,13,13,13,13,13,
    10,10,10,10,10,10,10,10,10,10
};

float center_line;
uint16 vision_curve_level = 0;
uint8 vision_weight_blend = 0;

// 图像巡线处理
#if IMAGE_COPY_ENABLE
uint8 far image_copy[MT9V03X_H][MT9V03X_W];
#endif

uint16 get_contrast(uint8 temp1,uint8 temp2)
{
    int16 diff = (int16)temp1 - (int16)temp2;
// 图像巡线处理
}


// 图像巡线处理
// 图像巡线处理
uint8 reference_point; // 图像巡线处理
uint8 white_max_point; // 图像巡线处理
uint8 white_min_point; // 图像巡线处理
void get_reference_point(const uint8 *image)
{
	uint16 i;
	uint8 *p= image+(SEARCH_IMAGE_H-REFRENCE_ROW)*SEARCH_IMAGE_W; // 图像巡线处理
	uint16 temp =0; // 图像巡线处理
	uint32 tempSum=0; // 图像巡线处理
	temp = REFRENCE_ROW * SEARCH_IMAGE_W; // 图像巡线处理
	for(i=0;i<temp;i++){
		tempSum+= *(p+i); // 图像巡线处理
	}
	reference_point = (uint8)(tempSum/temp); // 图像巡线处理
// 图像巡线处理
	white_max_point = (uint8)func_limit_ab(reference_point*WHITE_MAX_MUL/10,BALACK_POINT,230);
	white_min_point = (uint8)func_limit_ab(reference_point*WHITE_MIN_MUL/10,BALACK_POINT,230);
}



uint8 refenence_col_line[SEARCH_IMAGE_H]; // 图像巡线处理
uint8 reference_contrast_ratio = 32; // 图像巡线处理
uint8 reference_col; // 图像巡线处理
void Search_reference_col(const uint8 *image)
{
	uint8 col,row,temp1,temp2,i;
	uint8 globe_remote=120,globe_remote_min=120; // 图像巡线处理
	uint16 contrast=0;
	reference_col = SEARCH_IMAGE_W / 2;

// 图像巡线处理
	for(col=0;col<SEARCH_IMAGE_W;col+=PIXEL_OFFSET)
	{
		globe_remote=120; // 图像巡线处理
		for(row =SEARCH_IMAGE_H-1;row>PIXEL_OFFSET;row-=PIXEL_OFFSET)
		{
			temp1 =*(image + row*SEARCH_IMAGE_W+col); // 图像巡线处理
			temp2 =*(image + (row-PIXEL_OFFSET) * SEARCH_IMAGE_W +col); // 图像巡线处理

			if(temp2 > white_max_point) // 图像巡线处理
			{
				continue;
			}
				
			else if(temp1 < white_min_point)//判断为黑色
			{
				if(globe_remote>row)
					globe_remote=row;
				break;
			}
			
			contrast = get_contrast(temp1,temp2); // 图像巡线处理
			
			if(contrast > reference_contrast_ratio || row == STOP_ROW) // 图像巡线处理
			{
				if(globe_remote>row)
					globe_remote=row;
			}

		}	
// 图像巡线处理
			if(globe_remote<globe_remote_min)
			{
				globe_remote_min=globe_remote;
				reference_col= col; // 图像巡线处理
			}
				
	}
	
// 图像巡线处理
	for(i=0 ;i<SEARCH_IMAGE_H;i++) // 图像巡线处理
		refenence_col_line[i] = reference_col;
	
}



uint8 left_edge_line [SEARCH_IMAGE_H]; // 图像巡线处理
uint8 right_edge_line[SEARCH_IMAGE_H]; // 图像巡线处理
uint8 mid_line[SEARCH_IMAGE_H]; // 图像巡线处理
uint8 lost_left,lost_right; // 图像巡线处理
void Search_line(const uint8 *image)
{
	const uint8 *p = image; // 图像巡线处理
	uint8 row_max = SEARCH_IMAGE_H -1; // 图像巡线处理
	uint8 row_min = STOP_ROW;
	uint8 col_max = SEARCH_IMAGE_W - 1;
	uint8 col_min = 0;
	
	uint8 left_start_col  = reference_col; // 图像巡线处理
	uint8 right_start_col = reference_col+10; // 图像巡线处理
	uint8 left_end_col   = col_min; // 图像巡线处理
	uint8 right_end_col  = SEARCH_IMAGE_W-1; // 图像巡线处理

	uint8 search_time; // 图像巡线处理
	uint8 temp1,temp2; // 图像巡线处理
	int16 contrast;									//对比度
	
	uint8 left_stop=0,right_stop=0,stop_point=0; // 图像巡线处理
	uint8 col,row;
	
	lost_left=STOP_ROW;
	lost_right=STOP_ROW; // 图像巡线处理
	for(row = row_max;row >= row_min;row -- )
	{
		left_edge_line[row]  = 0;
		right_edge_line[row] = SEARCH_IMAGE_W-1;
	}
	//o_O("1.for");
	
	// 图像巡线处理
	for(row = row_max;  row >= row_min; row-=PIXEL_OFFSET)
	{
		p=image + row * SEARCH_IMAGE_W; // 图像巡线处理
		if(!left_stop) // 图像巡线处理
		{
			search_time =2;
			do
			{
				if(search_time == 1)
				{
					left_start_col  = reference_col;
					left_end_col    = col_min;
				}
				search_time--;
				// 图像巡线处理
				for(col = left_start_col;col > left_end_col+PIXEL_OFFSET;col-=PIXEL_OFFSET)
				{
					temp1 = *( p + col ); // 图像巡线处理
					temp2 = *( p + col-PIXEL_OFFSET); // 图像巡线处理

					// 图像巡线处理
					if(temp1 < white_min_point && col == left_start_col && left_start_col == reference_col)
					{
						left_stop =1;
						search_time=0; // 图像巡线处理
// 图像巡线处理
						for(stop_point = row;stop_point>1;stop_point--)
						{
							left_edge_line[stop_point] = col_min; // 图像巡线处理
						}
						lost_left=row; // 图像巡线处理
						break;
					}
// 图像巡线处理
					if(temp1 < white_min_point)
					{						
						left_edge_line[row]=col;						
						break;
					}
// 图像巡线处理
					if(temp2 > white_max_point)
					{
						continue;
					}
					// 图像巡线处理
					contrast = get_contrast(temp1,temp2); // 图像巡线处理
					
					if(contrast > reference_contrast_ratio || col == col_min)
					{
						left_edge_line[row]=col;
						left_start_col = (uint8)func_limit_ab(col+SEARCH_RANGE,col,col_max);
						left_end_col   = (uint8)func_limit_ab(col-SEARCH_RANGE,col_min,col); // 图像巡线处理
						search_time =0;
						break;
					}
				}
			}while(search_time);
		}
		
		if(!right_stop) // 图像巡线处理
		{
			search_time =2;
			do
			{
				if(search_time == 1)
				{
					right_start_col  = reference_col;
					right_end_col    = col_max;
				}
				search_time--;
				// 图像巡线处理
				for(col = right_start_col;col <=right_end_col-PIXEL_OFFSET;col+=PIXEL_OFFSET)
				{
					temp1 = *( p + col ); // 图像巡线处理
					temp2 = *( p + col + PIXEL_OFFSET); // 图像巡线处理

					// 图像巡线处理
					if(temp1 < white_min_point && col == right_start_col && right_start_col == reference_col)
					{
						right_stop =1;
						search_time=0; // 图像巡线处理
// 图像巡线处理
						for(stop_point=row;stop_point>row_min;stop_point--)
						{
							right_edge_line[stop_point] = SEARCH_IMAGE_W-1; // 图像巡线处理
						}
						lost_right=row; // 图像巡线处理
						break;
					}
// 图像巡线处理
					if(temp1 < white_min_point)
					{

						right_edge_line[row]=col;
						break;
					}
// 图像巡线处理
					if(temp2 > white_max_point)
					{
						continue;
					}
					// 图像巡线处理
					contrast = get_contrast(temp1,temp2);
					if(contrast > reference_contrast_ratio || col >= col_max - PIXEL_OFFSET)
					{
						
						right_edge_line[row]=col;
						right_start_col = (uint8)func_limit_ab(col-SEARCH_RANGE,col_min,col);
						right_end_col   = (uint8)func_limit_ab(col+SEARCH_RANGE,col,col_max); // 图像巡线处理
						search_time =0;
						break;
					}
				}
			}while(search_time);
		}		
	}
	
	// 图像巡线处理
	insert_val();
// 图像巡线处理
	memcpy(left_control_line, left_edge_line, sizeof(left_edge_line)); 
	memcpy(right_control_line,right_edge_line, sizeof(right_edge_line)); 
}

// 图像巡线处理
void insert_val(void)
{
	uint8 i,j;
	uint8 front_p,cur_p; // 图像巡线处理
	uint8 step;
	// 图像巡线处理
	front_p=left_edge_line[SEARCH_IMAGE_H-1];
	for(i = SEARCH_IMAGE_H-PIXEL_OFFSET-1;i>= STOP_ROW;i-=PIXEL_OFFSET){
		cur_p=left_edge_line[i];
		if(cur_p >= front_p){
			step=(cur_p-front_p+PIXEL_OFFSET-1)/PIXEL_OFFSET; // 图像巡线处理
			for(j=1;j<PIXEL_OFFSET;j++){
				left_edge_line[i-j+PIXEL_OFFSET]=front_p+step*j;
			}				
		}
		else{
			step=(front_p-cur_p+PIXEL_OFFSET-1)/PIXEL_OFFSET;
			for(j=1;j<PIXEL_OFFSET;j++){
				left_edge_line[i-j+PIXEL_OFFSET]=front_p-step*j;
			}
		}
		front_p=cur_p; // 图像巡线处理
	}
	
	//插值右边界
	front_p=right_edge_line[SEARCH_IMAGE_H-1];
	for(i = SEARCH_IMAGE_H-PIXEL_OFFSET-1;i>= STOP_ROW;i-=PIXEL_OFFSET){
		cur_p=right_edge_line[i];
		if(cur_p >= front_p){
			step=(cur_p-front_p+PIXEL_OFFSET-1)/PIXEL_OFFSET;
			for(j=1;j<PIXEL_OFFSET;j++){
				right_edge_line[i-j+PIXEL_OFFSET]=front_p+j*step;
			}				
		}
		else{
			step=(front_p-cur_p+PIXEL_OFFSET-1)/PIXEL_OFFSET;
			for(j=1;j<PIXEL_OFFSET;j++){
				right_edge_line[i-j+PIXEL_OFFSET]=front_p-j*step;
			}
		}
		front_p=cur_p;
	}
}

// 图像巡线处理
static int16 abs_i16_image(int16 x)
{
// 图像巡线处理
}

static uint8 clamp_blend_u16(uint16 x)
{
    if(x > 100)
    {
        return 100;
    }
    return (uint8)x;
}

static uint8 get_control_center(uint8 row)
{
    return (uint8)(((uint16)left_control_line[row] + (uint16)right_control_line[row]) >> 1);
}

static int16 avg_control_center(uint8 row_start, uint8 row_end)
{
    uint8 row;
    uint16 sum = 0;
    uint8 cnt = 0;

    if(row_end >= SEARCH_IMAGE_H)
    {
        row_end = SEARCH_IMAGE_H - 1;
    }

    for(row = row_start; row <= row_end; row++)
    {
        sum += get_control_center(row);
        cnt++;
    }

    if(cnt == 0)
    {
        return Mid_Col;
    }

    return (int16)(sum / cnt);
}

static uint8 map_curve_level_to_blend(uint16 level)
{
	// if(level <= 30)
	// {
	// 	return 0;
	// }

	// if(level >= 80)
	// {
	// 	return 100;
	// }

	// return clamp_blend_u16((uint16)((level - 30) * 100 / (80 - 30)));

    // if(level <= 45)
    // {
    //     return 0;
    // }

    // if(level >= 100)
    // {
    //     return 100;
    // }

    // return clamp_blend_u16((uint16)((level - 45) * 100 / (100 - 45)));

	if(level <= 60)
	{
		return 0;
	}

	if(level >= 120)
	{
		return 100;
	}

	return clamp_blend_u16((uint16)((level - 60) * 100 / (120 - 60)));
}

static void update_vision_weight_blend(void)
{
    static float vision_curve_filter = 0.0f;

    int16 far_mid;
    int16 mid_mid;
    int16 near_mid;
    int16 e_far;
    int16 e_mid;
    int16 e_near;
    int16 bend;
    uint16 raw_level;

    if(current_step > 0 && current_step <= 10)
    {
        vision_curve_filter = 0.0f;
        vision_curve_level = 0;
        vision_weight_blend = 0;
        return;
    }

    far_mid = avg_control_center(30, 45);
    mid_mid = avg_control_center(65, 85);
    near_mid = avg_control_center(90, 110);

    e_far = far_mid - Mid_Col;
    e_mid = mid_mid - Mid_Col;
    e_near = near_mid - Mid_Col;
    bend = e_near - e_far;

    raw_level = (uint16)(abs_i16_image(e_mid) * 2 + abs_i16_image(bend) * 2);

    vision_curve_filter = vision_curve_filter * 0.75f + (float)raw_level * 0.25f;
    vision_curve_level = (uint16)vision_curve_filter;
    vision_weight_blend = map_curve_level_to_blend(vision_curve_level);
}

static uint8 get_dynamic_weight(uint8 row)
{
    uint16 straight_part;
    uint16 curve_part;
    uint16 weight;

    if(current_step > 0 && current_step <= 10)
    {
        return ring_weight_array[row];
    }

    straight_part = (uint16)straight_weight_array[row] * (uint16)(100 - vision_weight_blend);
    curve_part = (uint16)curve_weight_array[row] * (uint16)vision_weight_blend;
    weight = (straight_part + curve_part + 50) / 100;

    if(weight == 0)
    {
        weight = 1;
    }

    return (uint8)weight;
}
void Fitted_Midline(void)
{
    uint8 i;
    uint8 row_weight;
    uint32 weight_sum = 0;
    float center;
    float sum = 0.0f;

    update_vision_weight_blend();

    for(i = 0; i < SEARCH_IMAGE_H; i++)
    {
        center = (float)get_control_center(i);
        row_weight = get_dynamic_weight(i);

        sum += (center - (float)Mid_Col) * (float)row_weight;
        weight_sum += row_weight;
        mid_line[i] = (uint8)center;
    }

    if(weight_sum == 0)
    {
        center_line = 0.0f;
    }
    else
    {
        center_line = sum / (float)weight_sum;
    }
}

int32 err_sum=0;
void Error_sum(void)
{
    uint8 i;
    int32 sum1 = 0, sum2 = 0, sum3 = 0;
    uint8 cnt1 = 0, cnt2 = 0, cnt3 = 0;

    for(i = 8; i < 28; i++) {
        sum1 += mid_line[i];
        cnt1++;
    }

    for(i = 28; i < 89; i++) {
        sum2 += mid_line[i];
        cnt2++;
    }

    for(i = 89; i < 111; i++) {
        sum3 += mid_line[i];
        cnt3++;
    }
	
	//2:5:3
    err_sum = ((sum1 / cnt1) * 20 + (sum2 / cnt2) * 50 + (sum3 / cnt3) * 30) / 100;
}


void straightAccelerate(void)
{
	uint8 isContinue_left,isContinue_right;
	isContinue_left=isContinueLine(left_edge_line);
	isContinue_right=isContinueLine(right_edge_line);

	if((isContinue_left && isContinue_right) )
	{
		Buzzer_On();
		servo_pidf.kp=15;
	}
	else 
	{
		Buzzer_Off();
		servo_pidf.kp=70;
	}
	
	
}

void Out_Check(const uint8* image)
{
    uint16 row, col;
    uint16 black_cnt = 0;
    uint16 total_cnt = 0;
    static uint8 out_cnt = 0;

    if(out_track_flag)
    {
        return;
    }

    for(row = 80; row < SEARCH_IMAGE_H; row += 2)
    {
        for(col = 0; col < SEARCH_IMAGE_W; col += 2)
        {
            total_cnt++;

            if(*(image + row * SEARCH_IMAGE_W + col) < 100)
            {
                black_cnt++;
            }
        }
    }

    if(total_cnt == 0)
    {
        return;
    }

    if((uint32)black_cnt * 100 >= (uint32)total_cnt * 88)
    {
        if(out_cnt < 4)
        {
            out_cnt++;
        }
    }
    else
    {
        out_cnt = 0;
    }

    if(out_cnt >= 4)
    {
        out_track_flag = 1;
    }
}
