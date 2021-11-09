#include "VirtualMemory.h"
#include "PhysicalMemory.h"
#include <cmath>


// the info struct

struct info
{
	bool fondUnusedFrame = false;
	bool removeFromParent = false;
	uint64_t max_frame = 0;
	int64_t weight = -1;
	uint64_t path[TABLES_DEPTH + 1] = {0};
	uint64_t page = 0;
};

//@@@@@@@@@@@@@@@@@@@@@@@@       function declaration       @@@@@@@@@@@@@@@@@@@@@@@@@//

void clearTable(uint64_t frameIndex);

uint64_t weight(int frameIdx, int page = -1);

uint64_t getPAdd(uint64_t virtualAddress);

info findUnusedFrame(int frameIdx, int depth, uint64_t VAdd, uint64_t page);

//@@@@@@@@@@@@@@@@@@@@@@@@      function implementation      @@@@@@@@@@@@@@@@@@@@@@@@@//


/**
 * this function clear the table
 * @param frameIndex - the index of the frame
 */
void clearTable(uint64_t frameIndex)
{
	for (uint64_t i = 0; i <  PAGE_SIZE; ++i)
	{
		PMwrite(frameIndex * PAGE_SIZE + i, 0);
	}
}


/**
 * this function will be called before any other function is called
 */
void VMinitialize()
{
	clearTable(0);
}


/**
 * this function get the Physical address of a giving virtual address
 * @param virtualAddress - the virtual Address
 * @return the Physical address
 */
uint64_t getPAdd(uint64_t virtualAddress)
{

    uint64_t pAdd;
    info res;
    word_t add = 0;
    uint64_t depth = TABLES_DEPTH;
    uint64_t i = 0;

    if (RAM_SIZE <= TABLES_DEPTH)
    {
        return 0;
    }
    while (i != depth)
    {
        pAdd = (add * PAGE_SIZE) +
               ((virtualAddress >> (TABLES_DEPTH * OFFSET_WIDTH - i * OFFSET_WIDTH)) &
                ((uint64_t) pow(2, OFFSET_WIDTH) - 1));
        PMread(pAdd, &add);
        if (add == 0)
        {
            res = findUnusedFrame(0, 0, virtualAddress, 0);
            if (res.fondUnusedFrame)
            {
                clearTable(res.max_frame);
                PMwrite(pAdd, res.max_frame);
                add = res.max_frame;
            }
            else
            {
                add = res.path[depth];
                PMevict(add, res.page);
                uint64_t m = res.path[depth - 1] * PAGE_SIZE +
                             ((res.page & ((uint64_t) pow(2, OFFSET_WIDTH) - 1)));
                PMwrite(m, 0);
                PMwrite(pAdd, add);
                clearTable(add);

            }
        }
        i++;
    }
    pAdd = ((virtualAddress >> OFFSET_WIDTH) & ((uint64_t) pow(2, TABLES_DEPTH*OFFSET_WIDTH) - 1));
    PMrestore(add, pAdd);
    uint64_t uAdd = add << OFFSET_WIDTH | (virtualAddress & ((uint64_t) pow(2, OFFSET_WIDTH) - 1));
    return uAdd;
}


/**
 * reads the word from the virtual address virtualAddressinto *value.
 * @param virtualAddress - the virtualAddress
 * @param value - the value that we store what we read
 * @return 1 on success and 0 on failure.
 */
int VMread(uint64_t virtualAddress, word_t *value)
{

    if(virtualAddress < 0 || virtualAddress >= VIRTUAL_MEMORY_SIZE)
    {
        return 0;
    }
    uint64_t PAdd = getPAdd(virtualAddress);
	PMread(PAdd, value);
	return 1;
}


/**
 * writes the word valueinto the virtual address virtualAddress.
 * @param virtualAddress - the virtualAddress that we writing to.
 * @param value - the value that we writing.
 * @return 1 on success and 0 on failure.
 */
int VMwrite(uint64_t virtualAddress, word_t value)
{
    if(virtualAddress < 0 || virtualAddress >= VIRTUAL_MEMORY_SIZE)
    {
        return 0;
    }
    uint64_t PAdd = getPAdd(virtualAddress);
    PMwrite(PAdd, value);
	return 1;
}


/**
 * this function find unused farame
 * @param frameIdx - the frame index
 * @param depth - the depth of the tree
 * @param VAdd - the virtualAddress
 * @param page - the page
 * @return info struct
 */
info findUnusedFrame(int frameIdx, int depth, uint64_t VAdd, uint64_t page)
{
	word_t value;
	int num_of_zeros = 0;
	info res;
	info midRes;
	if (depth == TABLES_DEPTH)
	{
		res.fondUnusedFrame = false;
		res.weight = weight(frameIdx, page);
		res.path[depth] = frameIdx;
		res.page = page;
		return res;
	}
	for (int pageIdx = 0; pageIdx < PAGE_SIZE; pageIdx++)
	{

		PMread(frameIdx * PAGE_SIZE + pageIdx, &value);
		if (value == 0)
		{
			num_of_zeros++;
		}
		else
		{
			if ((uint64_t) value > res.max_frame)
			{
				res.max_frame = value;
			}
			midRes = findUnusedFrame(value, depth + 1, VAdd, (page << OFFSET_WIDTH) | pageIdx);
			midRes.path[depth] = frameIdx;
			if (midRes.fondUnusedFrame)
			{
				if (midRes.removeFromParent)
				{
					PMwrite(frameIdx * PAGE_SIZE + pageIdx, 0);
					midRes.removeFromParent = false;
				}
				return midRes;
			}
			if (midRes.weight >= res.weight || res.weight == -1 ||
			(midRes.weight == res.weight && midRes.page > res.page))
			{
			    if (midRes.weight == res.weight)
                {

                }
				res.weight = midRes.weight;
				res.page = midRes.page;
				for (int j = 0; j < (TABLES_DEPTH + 1); j++)
				{
					res.path[j] = midRes.path[j];
				}
			}
			if (res.max_frame < midRes.max_frame)
			{
				res.max_frame = midRes.max_frame;
			}
		}
	}
	uint64_t h = (VAdd >> ((TABLES_DEPTH - (depth - 1)) * OFFSET_WIDTH)) &
				 ((uint64_t) pow(2, depth * OFFSET_WIDTH) - 1);

	if (num_of_zeros == PAGE_SIZE && frameIdx != 0 && h != page)
	{
		res.fondUnusedFrame = true;
		res.removeFromParent = true;
		res.max_frame = frameIdx;
		return res;
	}
	if (res.max_frame + 1 < NUM_FRAMES && frameIdx == 0)
	{
		res.fondUnusedFrame = true;
		res.max_frame += 1;
		return res;
	}
	if (res.weight != -1)
    {
        res.weight += weight(frameIdx);
    }
	return res;
}


/**
 * this function is helper to the findUnusedFrame function
 * @param frameIdx - the frame index
 * @param page - the page number
 * @return address
 */
uint64_t weight(int frameIdx, int page)
{
	uint64_t out;
	if (frameIdx % 2 == 0)
	{
		out = WEIGHT_EVEN;
	}
	else
	{
		out = WEIGHT_ODD;
	}
	if (page > -1)
	{
		if (page % 2 == 0)
		{
			out += WEIGHT_EVEN;
		}
		else
		{
			out += WEIGHT_ODD;
		}
	}
	return out;
}
