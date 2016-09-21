/*
 * ccnl_sensors.c
 *
 *  Created on: Sep 5, 2016
 *      Author: adeel
 */
#include "abe_relic.h"
#include "random.h"
#include "periph/gpio.h"

struct ccnl_resource {
	struct ccnl_resource *next, *prev;
	/* prefix will be of the form /accelerometer */
	char prefix[50];
	int prefix_len;
	char data[5 * 1024]; //Max ABE data = 10 KB
	int data_len;
	int latest_seq_no;
	int has_active_data;
};

struct ccnl_resource *ccnl_resource_head = NULL;
int ccnl_resources_initialized = 0;

#define MAX_CHUNK_SIZE 64

/*!
 * Checks whether the user button on the stm32f4-discovery board is pressed.
 * (need "periph/gpio.h" included).
 *
 * @note 		Need to call gpio_init(GPIO_PIN(PORT_A, 0), GPIO_IN), before 
 * 				first usage
 *
 * @return		Returns 1 if pressed, 0 otherwise
 */
//int read_button(void){
//	if (gpio_read(GPIO_PIN(PORT_A, 0)) != 0)
//		return 1;
//	return 0;
//}

/*!
 * Very elaborate function to simulate the current temperature, it is almost as
 * if it is the real temperature. (Ranges from 18 - 22) (need "random.h" included)
 *
 * @return 		An int representing the current temperature in Celsius
 */
int read_temperature(void){
	int base_temp = 20, sign, delta, temp;
	sign  = random_uint32() % 2;
	delta = random_uint32() % 3;

	if (sign)
		temp = base_temp - delta;
	else
		temp = base_temp + delta;

	return temp;
}

/*!
 * Very elaborate function to simulate the current CO2 level, it is almost as
 * if it is the real CO2 level. (Ranges from 395 - 403) (need "random.h" included)
 *
 * @return 		An int representing the current CO2 level in ppm
 */
int read_co2(void){
	int base_co2 = 399, sign, delta, co2_level;
	sign  = random_uint32() % 2;
	delta = random_uint32() % 5;

	if (sign)
		co2_level = base_co2 - delta;
	else
		co2_level = base_co2 + delta;

	return co2_level;
}

/*
 * This function will be called for each on-board sensor
 * only at initialization.
 *
 * Pass this function a null character terminated string.
 */
void ccnl_add_resource(char* prefix) {

	struct ccnl_resource *cr = (struct ccnl_resource *)ccnl_malloc(sizeof(struct ccnl_resource));

	cr->prefix_len = strlen(prefix);
	strcpy(cr->prefix, prefix);

	cr->next = cr->prev = NULL;
	//cr->data = NULL;
	cr->data_len = 0;
	cr->latest_seq_no = 0;
	cr->has_active_data = 0;

	if(ccnl_resource_head == NULL)
		ccnl_resource_head = cr;
	else
		DBL_LINKED_LIST_ADD(ccnl_resource_head, cr);
}

struct ccnl_resource* ccnl_get_resource(char *prefix) {
	struct ccnl_resource *cr = ccnl_resource_head;

	if(cr == NULL)
		return NULL;

	while(cr != NULL) {
		if(!strcmp(cr->prefix, prefix))
			return cr;
		cr = cr->next;
	}
	return NULL;
}

void ccnl_resource_init(void) {
	if(!ccnl_resources_initialized) {
		ccnl_add_resource("/co2");
		ccnl_add_resource("/temperature");
		ccnl_resources_initialized = 1;
	}
}

/* Simple generate_data function, remove this later */
void generate_data(char *data, int *datalen) {
	char huge_content[] = "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890";
	strcpy(data, huge_content);
	*datalen = strlen(data);
}

/*
 * Return 1 if the request is served, else return 0.
 */
int ccnl_resource_handleInterest(struct ccnl_relay_s *ccnl, int suite, struct ccnl_prefix_s *pfx) {
	ccnl_resource_init();

	/*
	 * url will be of the form /accelerometer/latest/%00 or /accelerometer/10/%00
	 * or /accelerometer/latest/%00%00 or /accelerometer/10/%00%00
	 */
	char *ccnl_url = ccnl_prefix_to_path(pfx);
	int url_len = strlen(ccnl_url);
	char url[url_len + 1];
	strcpy(url, ccnl_url);
	ccnl_free(ccnl_url);

	/*
	 * Some validation for the url.
	 * Should have at least 3 name segments.
	 */
	if(pfx->compcnt < 3)
		return 0;

	/*
	 * Extract last two name segments (e.g. "10", "%00%00").
	 * Extract the remaining URL (e.g. /kista/accelerometer).
	 */
	int slash_cnt = 0;
	char *index = url + 1;
	while(slash_cnt < pfx->compcnt - 2) {
		if(*index == '/')
			slash_cnt++;
		index++;
	}
	int remaining_name_seg_size = index -1 - url;
	char remaining_name_seg[remaining_name_seg_size  + 1];
	strncpy(remaining_name_seg, url, remaining_name_seg_size);
	remaining_name_seg[remaining_name_seg_size] = '\0';

	while(*index != '/')
		index++;
	int seclast_name_seg_size = (index - url) - remaining_name_seg_size - 1;
	char seclast_name_seg[seclast_name_seg_size + 1];
	strncpy(seclast_name_seg, url + remaining_name_seg_size + 1, seclast_name_seg_size);
	seclast_name_seg[seclast_name_seg_size] = '\0';

	index++;
	int last_name_seg_size = (url + url_len) - index;
	char last_name_seg[last_name_seg_size + 1];
	strcpy(last_name_seg, index);


	/*
	 * Some strings to work with
	 */
	char *latest_segstr = "latest";
	int latest_segstr_len = strlen(latest_segstr);

	/*
	 * Some more validation, ensure segment sizes > 0.
	 *
	 * Also ensure that second last segment is either a number or "latest",
	 * if it is a number convert it into an integer.
	 *
	 * With the chunking client, last segment does not need to be checked,
	 * it will always be of the form %00 or %00%00.
	 */
	if(last_name_seg_size <= 0 || seclast_name_seg_size <= 0)
		return 0;

	char *end;
	int seq_no_requested = strtol(seclast_name_seg, &end, 10);
	if( strcmp(seclast_name_seg, latest_segstr) &&
			((seclast_name_seg_size > 1 && seclast_name_seg[0] == '0') || end != seclast_name_seg + seclast_name_seg_size) )
		return 0;

	/*
	 * Now we check if the CCN resource exists at all.
	 * If yes, the resource pointer is retrieved from the linked list.
	 */
	struct ccnl_resource *cr;
	if((cr = ccnl_get_resource(remaining_name_seg)) == NULL)
		return 0;


	/*
	 * The following code block is to handle Interests for <....>/latest
	 */
	if(!strcmp(seclast_name_seg, latest_segstr) && *pfx->chunknum == 0) {
		char latest_seq_no_str[10];
		sprintf(latest_seq_no_str, "%d", cr->latest_seq_no);
		//itoa(cr->latest_seq_no, seq_no, 10);
		int seq_no_str_len = strlen(latest_seq_no_str);

		int content_len = remaining_name_seg_size + seq_no_str_len + 1;
		char content[content_len + 1];
		strncpy(content, remaining_name_seg, remaining_name_seg_size);
		content[remaining_name_seg_size] = '/';
		strcpy(content + remaining_name_seg_size + 1, latest_seq_no_str);

		ccnl_flush_cache(ccnl);

		/* url2 will be of the form /accelerometer/latest */
		char url2[remaining_name_seg_size + 1 + latest_segstr_len + 1];
		strcpy(url2, remaining_name_seg);
		url2[remaining_name_seg_size] = '/';
		strcpy(url2 + remaining_name_seg_size + 1, latest_segstr);
		ccnl_chunk_add2cache(ccnl, suite, url2, content, content_len, MAX_CHUNK_SIZE, 0);
		return 1;
	}

	/*
	 * The following code block is to handle requests for resource chunks
	 */

	/*
	 * First see if seq_no matches, if not quit function, if yes go ahead.
	 */
	if(seq_no_requested != cr->latest_seq_no)
		return 0;

	/*
	 * If chunk number == 0 and !has_active_data, generate data and set has_active_data.
	 * If chunk number > 0, and !has_active data then quit the function.
	 */
	if(*pfx->chunknum == 0) {
		if(!cr->has_active_data) {
			/* Replace generate_data() with Joakim's ABE encryption function */
//			generate_data(cr->data, &cr->data_len);
			cr->data_len = sizeof(cr->data);
			if (strcmp(cr->prefix, "/co2") == 0){
				format_symm_enc_latest_key((uint8_t*)cr->data, &cr->data_len,
						'c', read_co2());
			}else if (strcmp(cr->prefix, "/temperature") == 0){
				format_symm_enc_latest_key((uint8_t*)cr->data, &cr->data_len,
						't', read_temperature());
			}
			cr->has_active_data = 1;
		}
	}
	else {
		if(!cr->has_active_data)
			return 0;
	}


	/*
	 * Add chunk to cache.
	 * If this was last chunk set has_active_data to 0 and increment latest_seq_no
	 */

	/* url2 will be of the form /accelerometer/10 */
	char url2[remaining_name_seg_size + 1 + seclast_name_seg_size + 1];
	strcpy(url2, remaining_name_seg);
	url2[remaining_name_seg_size] = '/';
	strcpy(url2 + remaining_name_seg_size + 1, seclast_name_seg);
	ccnl_flush_cache(ccnl);
	ccnl_chunk_add2cache(ccnl, suite, url2, cr->data, cr->data_len, MAX_CHUNK_SIZE, *pfx->chunknum);

	if((*pfx->chunknum + 1) * MAX_CHUNK_SIZE >= cr->data_len) {
		cr->has_active_data = 0;
		cr->latest_seq_no++;
	}

	return 1;
}

