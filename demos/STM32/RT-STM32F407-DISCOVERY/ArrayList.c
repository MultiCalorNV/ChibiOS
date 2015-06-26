#include <stdlib.h>

#include "hal.h"
#include "mbconfig.h"
#include "ITM_trace.h"
#include "chprintf.h"

#include "ArrayList.h"
#include "ArrayUtil.h"

extern ITMStream itm_port;

/*	Static functions  ----------------------------------------------*/
static void printElement(const DeviceElement* const);
static void shift(DeviceList* const list, int index, int rooms, Shift dir);
static void wide(DeviceList* const);

void init(DeviceList* const list)
{
	initWithSize(list, MB_MASTER_TOTAL_SLAVE_NUM);
}

void initWithSize(DeviceList* const list, int size)
{
	initWithSizeAndIncRate(list, size, 1);
}

void initWithSizeAndIncRate(DeviceList* const list, int size, int rate)
{
	list->size = size;
	list->increment_rate = rate;
	list->elements = (DeviceElement*) chHeapAlloc( NULL, list->size * sizeof(DeviceElement));
	list->current = -1;
}

void clear(DeviceList* const list)
{
	while(list->current >= 0)
	{
		list->elements[list->current] = (DeviceElement){0};
		list->current--;
	}
}

int set(DeviceList* const list, DeviceElement e, int index)
{
	if(index <= list->current)
	{
		list->elements[index] = e;
	}
	return 0;
}

DeviceElement* get(DeviceList* const list, int index)
{
	if(index <= list->current)
	{
		DeviceElement *e = &list->elements[index];
		return e;
	}
	return NULL;
}

int get_Index(DeviceList* const list, int8_t address)
{
	int index = 0;
	while(index <= list->current)
	{
		if((list->elements[index].address) == address) return index;
		index++;
	}
	return -1;
}

int add(DeviceList* const list, DeviceElement e)
{
	uint8_t i;
	
	for(i = 0; i <= list->current; i++)
	{
		if(list->elements[i].address == e.address)
			return;
	}
	if(++list->current < list->size)
	{
		list->elements[list->current] = e;
		return 1;
	}else
	{
		wide(list);
		list->elements[list->current] = e;
		return 1;
	}
	return 0;
}

static void wide(DeviceList* const list)
{
	list->size += list->increment_rate;
	DeviceElement *newArr = (DeviceElement*) chHeapAlloc( NULL, list->size * sizeof(DeviceElement));
	arrayCopy(newArr, 0, list->elements, 0, list->current, list->size, sizeof(DeviceElement));
	free(list->elements);
	list->elements = newArr;
}

int insert(DeviceList* const list, DeviceElement e, int index)
{
	uint8_t i;
	
	for(i = 0; i <= list->current; i++)
	{
		if(list->elements[i].address == e.address)
			return;
	}
	
	if(index <= list->current && ++list->current < list->size)
	{
		shift(list, index, 1, RIGHT);
		list->elements[index] = e;
		return 1;
	}
	return 0;
}

int populate(DeviceList* const list, DeviceElement e)
{
	int index = 0;
	
	if(isEmpty(list) == -1) return -1;
	
	index = get_Index(list, e.address);
	if(index == -1){
		add(list, e);
	}else
	{
		set(list, e, index);
	}
	return 0;
}

int lastIndexOf(const DeviceList* const list, DeviceElement e)
{
	int index = list->current;
	while(index > -1)
	{
		if(e.address == list->elements[index].address) return(list->current - index);
		index--;
	}
	return 0;
}

int indexOf(const DeviceList* const list, DeviceElement e)
{
	int index = 0;
	while(index <= list->current)
	{
		if(e.address == list->elements[index].address) return index;
		index++;
	}
	return 0;
}

int isEmpty(const DeviceList* const list)
{
	return list->current == -1;
}

DeviceElement *removeAt(DeviceList* const list, int index)
{
	if(list->current >= index)
	{
		DeviceElement *e = &list->elements[index];
		shift(list, index, 1, LEFT);
		list->current--;
		return e;
	}
	return NULL;
}

void print(const DeviceList* const list)
{
	int i;
	
	for(i = 0; i <= list->current; i++)
	{
		DeviceElement e = list->elements[i];
		printElement(&e);
	}
	chprintf((BaseSequentialStream *)&itm_port, "\n");
}

void clean(DeviceList* list)
{
	free(list->elements);
}

static void printElement(const DeviceElement* const e)
{
	chprintf((BaseSequentialStream *)&itm_port, "print e->name: %s ", e->name);
	chprintf((BaseSequentialStream *)&itm_port, "print e->address: %i ", e->address);
}

static void shift(DeviceList* const list, int index, int rooms, Shift dir)
{
	if(dir == RIGHT)
	{
		arrayCopy(list->elements, index + 1, list->elements, index, rooms, list->current, sizeof(DeviceElement));
	}else //SHIFT
	{
		arrayCopy(list->elements, index, list->elements, index + 1, rooms, list->current, sizeof(DeviceElement));
	}
}