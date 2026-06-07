#ifndef CMD_HANDLER_H
#define CMD_HANDLER_H


#define CMD_MSG_LEN 2
#define CMD_MSG_START 123
#define CMD_MSG_END 125

// TODO: Come up with proper command ids.
enum {
	ARM = 1,
	DISARM,
	SET_FREQ,
	EN_PIN,
	DS_PIN,
	NUM_CMDS,
};

enum {
	CMD_SUCESS = NUM_CMDS + 1,
	CMD_FAIL,
	CMD_UNKNOWN,
};

void cmd_handler_task(void *params);

#endif
