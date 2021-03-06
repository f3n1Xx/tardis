/*
 * free.c
 *
 *  Created on: Sep 26, 2017
 *      Author: tardis
 */

#include <stdio.h>
#include "vh/vh_hash.h"
#include "free.h"
#include "processrefgen.h"
#include "vh/vh_divethandler.h"

void free_alignments( bam_alignment_region** bam_align)
{
	//free((*bam_align)->cigar);
	free((*bam_align)->read_name);
	free( *bam_align);
	*bam_align = NULL;
}

void free_alignments2( bam_alignment_region** bam_align)
{
	free((*bam_align)->cigar);
	free((*bam_align)->read_name);
	free( *bam_align);
	*bam_align = NULL;
}

void free_mappings( bam_info** in_bams, parameters* params)
{
	int i, k, bam_index;
	struct LibraryInfo *cursor, *t;

	softClip *sfcPtr, *sfcPtrNext;
	discordantMapping *ptrDisMap, *ptrDisMapNext;
	alternativeMapping *ptrAltMap, *ptrAltMapNext;
	discordantMappingMEI *ptrMEIMap, *ptrMEIMapNext;
	discordantMappingNUMT *ptrNUMTMap, *ptrNUMTMapNext;

	for( bam_index = 0; bam_index < params->num_bams; bam_index++)
	{
		for( i = 0; i < in_bams[bam_index]->num_libraries; i++)
		{
			ptrMEIMap = in_bams[bam_index]->libraries[i]->listMEI_Mapping;
			while( ptrMEIMap != NULL)
			{
				ptrMEIMapNext = ptrMEIMap->next;
				if( ptrMEIMap->chromosome_name != NULL)
					free( ptrMEIMap->chromosome_name);
				if( ptrMEIMap->readName != NULL)
					free( ptrMEIMap->readName);
				if( ptrMEIMap->MEI_subclass != NULL)
					free( ptrMEIMap->MEI_subclass);
				if( ptrMEIMap != NULL)
					free( ptrMEIMap);
				ptrMEIMap = ptrMEIMapNext;
			}
			in_bams[bam_index]->libraries[i]->listMEI_Mapping = NULL;

			ptrNUMTMap = in_bams[bam_index]->libraries[i]->listNUMT_Mapping;
			while( ptrNUMTMap != NULL)
			{
				ptrNUMTMapNext = ptrNUMTMap->next;
				if( ptrNUMTMap->chromosome_name != NULL)
					free( ptrNUMTMap->chromosome_name);
				if( ptrNUMTMap->readName != NULL)
					free( ptrNUMTMap->readName);
				if( ptrNUMTMap != NULL)
					free( ptrNUMTMap);
				ptrNUMTMap = ptrNUMTMapNext;
			}
			in_bams[bam_index]->libraries[i]->listNUMT_Mapping = NULL;

			if( !params->no_soft_clip)
			{
				sfcPtr = in_bams[bam_index]->libraries[i]->listSoftClip;
				while( sfcPtr != NULL)
				{
					sfcPtrNext = sfcPtr->next;
					if( sfcPtr->readName != NULL)
						free( sfcPtr->readName);
					if( sfcPtr->chromosome_name != NULL)
						free( sfcPtr->chromosome_name);
					if( sfcPtr->softClipString != NULL)
						free( sfcPtr->softClipString);
					free( sfcPtr);
					sfcPtr = sfcPtrNext;
				}
				in_bams[bam_index]->libraries[i]->listSoftClip = NULL;
			}

			for( k = 0; k < NHASH; k++)
			{
				ptrDisMap = in_bams[bam_index]->libraries[i]->mappings_discordant[k];
				while( ptrDisMap != NULL)
				{
					ptrDisMapNext = ptrDisMap->next;
					if( ptrDisMap->chromosome_name != NULL)
						free( ptrDisMap->chromosome_name);
					if( ptrDisMap->readName != NULL)
						free( ptrDisMap->readName);
					if( ptrDisMap != NULL)
						free( ptrDisMap);
					ptrDisMap = ptrDisMapNext;
				}
				in_bams[bam_index]->libraries[i]->mappings_discordant[k] = NULL;
			}
			if( params->alt_mapping != 0)
			{
				for( k = 0; k < NHASH; k++)
				{
					ptrAltMap = in_bams[bam_index]->libraries[i]->mappings_alternative[k];
					while( ptrAltMap != NULL)
					{
						ptrAltMapNext = ptrAltMap->next;
						if( ptrAltMap->chromosome_name != NULL)
							free( ptrAltMap->chromosome_name);
						if( ptrAltMap->readName != NULL)
							free( ptrAltMap->readName);
						if( ptrAltMap != NULL)
							free( ptrAltMap);
						ptrAltMap = ptrAltMapNext;
					}
					in_bams[bam_index]->libraries[i]->mappings_alternative[k] = NULL;
				}
			}
		}
	}
}

void free_libraries()
{
	int i;
	struct LibraryInfo *cursor, *t;
	LibraryInfo *libInfo, *libInfoNext;
	DivetRow *tmp, *tmp_next;
	ReadName *tmp2, *tmp2_next;

	libInfo = g_libInfo;
	while( libInfo != NULL)
	{
		libInfoNext = libInfo->next;
		for( i = 0; i < NHASH; i++)
		{
			if( libInfo->hash != NULL)

				tmp2 = libInfo->hash[i];
				while( tmp2 != NULL)
				{
					tmp2_next = tmp2->next;
					if( tmp2->readName != NULL)
						free( tmp2->readName);
					free( tmp2);
					tmp2 = tmp2_next;
				}
		}
		if( libInfo->hash != NULL)
			free( libInfo->hash);

		tmp = libInfo->head;
		while( tmp != NULL)
		{
			tmp_next = tmp->next;
			free( tmp->chromosome_name);
			free( tmp);
			tmp = tmp_next;
		}

		libInfo->head = NULL;
		libInfo->tail = NULL;
		libInfo->hash = NULL;
		free( libInfo);
		libInfo = libInfoNext;
	}
	g_libInfo = NULL;
}

void free_the_rest( bam_info** in_bams, parameters* params)
{
	int bam_index;

	for( bam_index = 0; bam_index < params->num_bams; bam_index++)
	{
		/* Free the read depth array*/
		free( in_bams[bam_index]->read_depth_per_chr);
	}
}


void free_sensitive(bam_info** in_bams, parameters *params)
{
	int i, j;


	/* Free the RD array */
	for( i = 0; i < params->num_bams; i++)
	{
		for( j = params->first_chr; j <= params->last_chr; j++)
			free( in_bams[i]->read_depth[j]);
		free( in_bams[i]->read_depth);
	}


	for( i = 0; i < params->num_bams; i++)
	{
		free( in_bams[i]->sample_name);
		for( j = 0; j < in_bams[i]->num_libraries; j++)
		{
			free( in_bams[i]->libraries[j]->divet);
			free( in_bams[i]->libraries[j]->fastq1);
			free( in_bams[i]->libraries[j]->fastq2);
			free( in_bams[i]->libraries[j]->libname);
			free( in_bams[i]->libraries[j]->listSoftClip);
		}
		free( in_bams[i]);
	}
	free( in_bams);

	free( params->bam_files);
	free( params->bam_list_path);
	free( params->dups);
	free( params->gaps);
	free( params->mei);
	free( params->outprefix);
	free( params->ref_genome);
	free( params->reps);
	free( params->sonic_file);
	free( params);
}

void free_quick(bam_info** in_bams, parameters *params)
{
	int i, j;

	/* Free bams and related libraries */
	for( i = 0; i < params->num_bams; i++)
	{
		free( in_bams[i]->sample_name);
		for( j = 0; j < in_bams[i]->num_libraries; j++)
		{
			free( in_bams[i]->libraries[j]->divet);
			free( in_bams[i]->libraries[j]->fastq1);
			free( in_bams[i]->libraries[j]->fastq2);
			free( in_bams[i]->libraries[j]->libname);
			free( in_bams[i]->libraries[j]->listSoftClip);
			free( in_bams[i]->libraries[j]->listMEI_Mapping);
		}
		free( in_bams[i]);
	}
	free( in_bams);

	/* Free params struct */
	free( params->bam_files);
	free( params->bam_list_path);
	free( params->dups);
	free( params->gaps);
	free( params->mei);
	free( params->outprefix);
	free( params->reps);
	free( params->sonic_file);
	free( params);
}
