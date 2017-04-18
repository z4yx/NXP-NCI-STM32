/*
*         Copyright (c), NXP Semiconductors Caen / France
*
*                     (C)NXP Semiconductors
*       All rights are reserved. Reproduction in whole or in part is
*      prohibited without the written consent of the copyright owner.
*  NXP reserves the right to make changes without notice at any time.
* NXP makes no warranty, expressed, implied or statutory, including but
* not limited to any implied warranty of merchantability or fitness for any
*particular purpose, or that the use will not infringe any third party patent,
* copyright or trademark. NXP must not be liable for any loss or damage
*                          arising from its use.
*/

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "tool.h"
#include "Nfc.h"
#include "ndef_helper.h"

#define print_buf(x,y,z)  {int loop; printf(x); for(loop=0;loop<z;loop++) printf("0x%.2x ", y[loop]); printf("\n");}

/* Discovery loop configuration according to the targeted modes of operation */
unsigned char DiscoveryTechnologies[] = {
#if defined P2P_SUPPORT || defined RW_SUPPORT
	MODE_POLL | TECH_PASSIVE_NFCA,
	MODE_POLL | TECH_PASSIVE_NFCF,
#endif
#ifdef RW_SUPPORT
	MODE_POLL | TECH_PASSIVE_NFCB,
	MODE_POLL | TECH_PASSIVE_15693,
#endif
#ifdef P2P_SUPPORT
	MODE_POLL | TECH_ACTIVE_NFCF,
#endif
#if defined P2P_SUPPORT || defined CARDEMU_SUPPORT
	MODE_LISTEN | TECH_PASSIVE_NFCA,
#endif
#if defined CARDEMU_SUPPORT
	MODE_LISTEN | TECH_PASSIVE_NFCB,
#endif
#ifdef P2P_SUPPORT
	MODE_LISTEN | TECH_PASSIVE_NFCF,
	MODE_LISTEN | TECH_ACTIVE_NFCA,
	MODE_LISTEN | TECH_ACTIVE_NFCF,
#endif
};

/* Mode configuration according to the targeted modes of operation */
unsigned mode = 0
#ifdef CARDEMU_SUPPORT
			  | NXPNCI_MODE_CARDEMU
#endif
#ifdef P2P_SUPPORT
			  | NXPNCI_MODE_P2P
#endif
#ifdef RW_SUPPORT
			  | NXPNCI_MODE_RW
#endif
;

#if defined P2P_SUPPORT || defined RW_SUPPORT
void NdefPull_Cb(unsigned char *pNdefMessage, unsigned short NdefMessageSize)
{
	unsigned char *pNdefRecord = pNdefMessage;
	NdefRecord_t NdefRecord;
	unsigned char save;

	if (pNdefMessage == NULL)
	{
		printf("--- Issue during NDEF message reception (check provisioned buffer size) \n");
		return;
	}

	while (pNdefRecord != NULL)
	{
		printf("--- NDEF record received:\n");

		NdefRecord = DetectNdefRecordType(pNdefRecord);

		switch(NdefRecord.recordType)
		{
		case MEDIA_VCARD:
			{
				save = NdefRecord.recordPayload[NdefRecord.recordPayloadSize];
				NdefRecord.recordPayload[NdefRecord.recordPayloadSize] = '\0';
				printf("   vCard:\n%s\n", NdefRecord.recordPayload);
				NdefRecord.recordPayload[NdefRecord.recordPayloadSize] = save;
			}
			break;

		case WELL_KNOWN_SIMPLE_TEXT:
			{
				save = NdefRecord.recordPayload[NdefRecord.recordPayloadSize];
				NdefRecord.recordPayload[NdefRecord.recordPayloadSize] = '\0';
				printf("   Text record: %s\n", &NdefRecord.recordPayload[NdefRecord.recordPayload[0]+1]);
				NdefRecord.recordPayload[NdefRecord.recordPayloadSize] = save;
			}
			break;

		case WELL_KNOWN_SIMPLE_URI:
			{
				save = NdefRecord.recordPayload[NdefRecord.recordPayloadSize];
				NdefRecord.recordPayload[NdefRecord.recordPayloadSize] = '\0';
				printf("   URI record: %s%s\n", ndef_helper_UriHead(NdefRecord.recordPayload[0]), &NdefRecord.recordPayload[1]);
				NdefRecord.recordPayload[NdefRecord.recordPayloadSize] = save;
			}
			break;

		case MEDIA_HANDOVER_WIFI:
			{
				unsigned char index = 26, i;

				printf ("--- Received WIFI credentials:\n");
				if ((pNdefRecord[index] == 0x10) && (pNdefRecord[index+1] == 0x0e)) index+= 4;
				while(index < NdefRecord.recordPayloadSize)
				{
					if (pNdefRecord[index] == 0x10)
					{
						if (pNdefRecord[index+1] == 0x45) {printf ("- SSID = "); for(i=0;i<pNdefRecord[index+3];i++) printf("%c", pNdefRecord[index+4+i]); printf ("\n");}
						else if (pNdefRecord[index+1] == 0x03) printf ("- Authenticate Type = %s\n", ndef_helper_WifiAuth(pNdefRecord[index+5]));
						else if (pNdefRecord[index+1] == 0x0f) printf ("- Encryption Type = %s\n", ndef_helper_WifiEnc(pNdefRecord[index+5]));
						else if (pNdefRecord[index+1] == 0x27) {printf ("- Network key = "); for(i=0;i<pNdefRecord[index+3];i++) printf("#"); printf ("\n");}
						index += 4 + pNdefRecord[index+3];
					}
					else continue;
				}
			}
			break;

		default:
			printf("   Unsupported NDEF record, cannot parse\n");
			break;
		}
		pNdefRecord = GetNextRecord(pNdefRecord);
	}

	printf("\n");
}
#endif

#if defined P2P_SUPPORT || defined CARDEMU_SUPPORT
const char NDEF_MESSAGE[] = { 0xD1,   // MB/ME/CF/1/IL/TNF
		0x01,   // TYPE LENGTH
		0x07,   // PAYLOAD LENTGH
		'T',    // TYPE
		0x02,   // Status
		'e', 'n', // Language
		'T', 'e', 's', 't' };

void NdefPush_Cb(unsigned char *pNdefRecord, unsigned short NdefRecordSize) {
	printf("--- NDEF Record sent\n\n");
}
#endif

#if defined RW_SUPPORT
#ifdef RW_RAW_EXCHANGE
void MIFARE_scenario (void)
{
	bool status;
	unsigned char i;
	unsigned char Resp[256];
	unsigned char RespSize;
	/* Authenticate sector 1 with generic keys */
	unsigned char Auth[] = {0x40, 0x01, 0x10, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	/* Read block 4 */
	unsigned char Read[] = {0x10, 0x30, 0x04};
	/* Write block 4 */
	unsigned char WritePart1[] = {0x10, 0xA0, 0x04};
	unsigned char WritePart2[] = {0x10, 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};

	/* Authenticate */
	status = NxpNci_ReaderTagCmd(Auth, sizeof(Auth), Resp, &RespSize);
	if((status == NFC_ERROR) || (Resp[RespSize-1] != 0))
	{
		printf(" Authenticate sector %d failed with error 0x%02x\n", Auth[1], Resp[RespSize-1]);
		return;
	}
	printf(" Authenticate sector %d succeed\n", Auth[1]);

	/* Read block */
	status = NxpNci_ReaderTagCmd(Read, sizeof(Read), Resp, &RespSize);
	if((status == NFC_ERROR) || (Resp[RespSize-1] != 0))
	{
		printf(" Read block %d failed with error 0x%02x\n", Read[2], Resp[RespSize-1]);
		return;
	}
	printf(" Read block %d: ", Read[2]); for(i=0;i<RespSize-2;i++) printf("0x%02X ", Resp[i+1]); printf("\n");

	/* Write block */
	status = NxpNci_ReaderTagCmd(WritePart1, sizeof(WritePart1), Resp, &RespSize);
	if((status == NFC_ERROR) || (Resp[RespSize-1] != 0))
	{
		printf(" Write block %d failed with error 0x%02x\n", WritePart1[2], Resp[RespSize-1]);
		return;
	}
	status = NxpNci_ReaderTagCmd(WritePart2, sizeof(WritePart2), Resp, &RespSize);
	if((status == NFC_ERROR) || (Resp[RespSize-1] != 0))
	{
		printf(" Write block %d failed with error 0x%02x\n", WritePart1[2], Resp[RespSize-1]);
		return;
	}
	printf(" Block %d written\n", WritePart1[2]);

	/* Read block */
	status = NxpNci_ReaderTagCmd(Read, sizeof(Read), Resp, &RespSize);
	if((status == NFC_ERROR) || (Resp[RespSize-1] != 0))
	{
		printf(" Read failed with error 0x%02x\n", Resp[RespSize-1]);
		return;
	}
	printf(" Read block %d: ", Read[2]); for(i=0;i<RespSize-2;i++) printf("0x%02X ", Resp[i+1]); printf("\n");
}


void ISO15693_scenario (void)
{
	bool status;
	unsigned char i;
	unsigned char Resp[256];
	unsigned char RespSize;
	unsigned char ReadBlock[] = {0x02, 0x20, 0x08};
	unsigned char WriteBlock[] = {0x02, 0x21, 0x08, 0x11, 0x22, 0x33, 0x44};

	status = NxpNci_ReaderTagCmd(ReadBlock, sizeof(ReadBlock), Resp, &RespSize);
	if((status == NFC_ERROR) || (Resp[RespSize-1] != 0x00))
	{
		printf(" Read block %d failed with error 0x%02x\n", ReadBlock[2], Resp[RespSize-1]);
		return;
	}
	printf(" Read block %d: ", ReadBlock[2]); for(i=0;i<RespSize-2;i++) printf("0x%02X ", Resp[i+1]); printf("\n");

	/* Write */
	status = NxpNci_ReaderTagCmd(WriteBlock, sizeof(WriteBlock), Resp, &RespSize);
	if((status == NFC_ERROR) || (Resp[RespSize-1] != 0))
	{
		printf(" Write block %d failed with error 0x%02x\n", WriteBlock[2], Resp[RespSize-1]);
		return;
	}
	printf(" Block %d written\n", WriteBlock[2]);

	/* Read back */
	status = NxpNci_ReaderTagCmd(ReadBlock, sizeof(ReadBlock), Resp, &RespSize);
	if((status == NFC_ERROR) || (Resp[RespSize-1] != 0x00))
	{
		printf(" Read block %d failed with error 0x%02x\n", ReadBlock[2], Resp[RespSize-1]);
		return;
	}
	printf(" Read block %d: ", ReadBlock[2]); for(i=0;i<RespSize-2;i++) printf("0x%02X ", Resp[i+1]); printf("\n");
}

void PCD_ISO14443_3A_scenario (void)
{
	bool status;
	unsigned char i;
	unsigned char Resp[256];
	unsigned char RespSize;
	/* Read block 4 */
	unsigned char Read[] = {0x30, 0x04};
	/* Write block 4 */
	unsigned char Write[] = {0xA2, 0x04, 0x11, 0x22, 0x33, 0x44};

	/* Read */
	status = NxpNci_ReaderTagCmd(Read, sizeof(Read), Resp, &RespSize);
	if((status == NFC_ERROR) || (Resp[RespSize-1] != 0))
	{
		printf(" Read block %d failed with error 0x%02x\n", Read[1], Resp[RespSize-1]);
		return;
	}
	printf(" Read block %d: ", Read[1]); for(i=0;i<RespSize-2;i++) printf("0x%02X ", Resp[i+1]); printf("\n");

	/* Write */
	status = NxpNci_ReaderTagCmd(Write, sizeof(Write), Resp, &RespSize);
	if((status == NFC_ERROR) || (Resp[RespSize-1] != 0))
	{
		printf(" Write block %d failed with error 0x%02x\n", Write[1], Resp[RespSize-1]);
		return;
	}
	printf(" Block %d written\n", Write[1]);

	/* Read back */
	status = NxpNci_ReaderTagCmd(Read, sizeof(Read), Resp, &RespSize);
	if((status == NFC_ERROR) || (Resp[RespSize-1] != 0))
	{
		printf(" Read block %d failed with error 0x%02x\n", Read[1], Resp[RespSize-1]);
		return;
	}
	printf(" Read block %d: ", Read[1]); for(i=0;i<RespSize-2;i++) printf("0x%02X ", Resp[i+1]); printf("\n");
}

void PCD_ISO14443_4_scenario (void)
{
	bool status;
	unsigned char Resp[256];
	unsigned char RespSize;
	uint8_t SelectCmd[] = {0x00,0xa4,0x04,0x00,0x09,0xa0,0x00,0x00,0x00,0x03,0x86,0x98,0x07,0x01};
	uint8_t ReadFile[] = {0x00,0xb0,0x96,0x1c,0x0a};
//	unsigned char SelectPPSE[] = {0x00, 0xA4, 0x04, 0x00, 0x0E, 0x32, 0x50, 0x41, 0x59, 0x2E, 0x53, 0x59, 0x53, 0x2E, 0x44, 0x44, 0x46, 0x30, 0x31, 0x00};

	status = NxpNci_ReaderTagCmd(SelectCmd, sizeof(SelectCmd), Resp, &RespSize);
	if((status == NFC_ERROR) || (Resp[RespSize-2] != 0x90) || (Resp[RespSize-1] != 0x00))
	{
		printf(" Select PPSE failed with error %02x %02x\n", Resp[RespSize-2], Resp[RespSize-1]);
		return;
	}
	printf(" Select NDEF Application succeed, RespSize = %d\n", RespSize);

    status = NxpNci_ReaderTagCmd(ReadFile, sizeof(ReadFile), Resp, &RespSize);
    if((status == NFC_ERROR) || (Resp[RespSize-2] != 0x90) || (Resp[RespSize-1] != 0x00))
    {
        printf(" Read file failed with error %02x %02x\n", Resp[RespSize-2], Resp[RespSize-1]);
        return;
    }
    Resp[RespSize-2] = '\0';
    printf("Student Number = %s\n", Resp);
}

#endif

void displayCardInfo(NxpNci_RfIntf_t RfIntf)
{
	switch(RfIntf.Protocol){
	case PROT_T1T:
	case PROT_T2T:
	case PROT_T3T:
	case PROT_ISODEP:
		printf(" - POLL MODE: Remote T%dT activated\n", RfIntf.Protocol);
		break;
	case PROT_ISO15693:
		printf(" - POLL MODE: Remote ISO15693 card activated\n");
		break;
	case PROT_MIFARE:
		printf(" - POLL MODE: Remote MIFARE card activated\n");
		break;
	default:
		printf(" - POLL MODE: Undetermined target\n");
		return;
	}

	switch(RfIntf.ModeTech) {
	case (MODE_POLL | TECH_PASSIVE_NFCA):
		printf("\tSENS_RES = 0x%.2x 0x%.2x\n", RfIntf.Info.NFC_APP.SensRes[0], RfIntf.Info.NFC_APP.SensRes[1]);
		print_buf("\tNFCID = ", RfIntf.Info.NFC_APP.NfcId, RfIntf.Info.NFC_APP.NfcIdLen);
		if(RfIntf.Info.NFC_APP.SelResLen != 0) printf("\tSEL_RES = 0x%.2x\n", RfIntf.Info.NFC_APP.SelRes[0]);
	break;

	case (MODE_POLL | TECH_PASSIVE_NFCB):
		if(RfIntf.Info.NFC_BPP.SensResLen != 0) print_buf("\tSENS_RES = ", RfIntf.Info.NFC_BPP.SensRes, RfIntf.Info.NFC_BPP.SensResLen);
	break;

	case (MODE_POLL | TECH_PASSIVE_NFCF):
		printf("\tBitrate = %s\n", (RfIntf.Info.NFC_FPP.BitRate==1)?"212":"424");
		if(RfIntf.Info.NFC_FPP.SensResLen != 0) print_buf("\tSENS_RES = ", RfIntf.Info.NFC_FPP.SensRes, RfIntf.Info.NFC_FPP.SensResLen);
	break;

	case (MODE_POLL | TECH_PASSIVE_15693):
		print_buf("\tID = ", RfIntf.Info.NFC_VPP.ID, sizeof(RfIntf.Info.NFC_VPP.ID));
		printf("\tAFI = 0x%.2x\n", RfIntf.Info.NFC_VPP.AFI);
		printf("\tDSFID = 0x%.2x\n", RfIntf.Info.NFC_VPP.DSFID);
	break;

	default:
		break;
	}
}

void task_nfc_reader(NxpNci_RfIntf_t RfIntf)
{
	/* For each discovered cards */
	while(1){
		/* Display detected card information */
		displayCardInfo(RfIntf);

		/* What's the detected card type ? */
		switch(RfIntf.Protocol) {
		case PROT_T1T:
		case PROT_T2T:
		case PROT_T3T:
		case PROT_ISODEP:
#ifndef RW_RAW_EXCHANGE
			/* Process NDEF message read */
			NxpNci_ProcessReaderMode(RfIntf, READ_NDEF);
#ifdef RW_NDEF_WRITING
			RW_NDEF_SetMessage ((unsigned char *) NDEF_MESSAGE, sizeof(NDEF_MESSAGE), *NdefPush_Cb);
			/* Process NDEF message write */
			NxpNci_ProcessReaderMode(RfIntf, WRITE_NDEF);
#endif
#else
			if (RfIntf.Protocol == PROT_ISODEP)
			{
				PCD_ISO14443_4_scenario();
			}
			else if (RfIntf.Protocol == PROT_T2T)
			{
				PCD_ISO14443_3A_scenario();
			}
#endif
			break;

		case PROT_ISO15693:
#ifdef RW_RAW_EXCHANGE
			/* Run dedicated scenario to demonstrate ISO15693 card management */
			ISO15693_scenario();
#endif
			break;

		case PROT_MIFARE:
#ifdef RW_RAW_EXCHANGE
			/* Run dedicated scenario to demonstrate MIFARE card management */
			MIFARE_scenario();
#endif
			break;

		default:
			break;
		}

		/* If more cards (or multi-protocol card) were discovered (only same technology are supported) select next one */
		if(RfIntf.MoreTags) {
			if(NxpNci_ReaderActivateNext(&RfIntf) == NFC_ERROR) break;
		}
		/* Otherwise leave */
		else break;
	}

	/* Wait for card removal */
	NxpNci_ProcessReaderMode(RfIntf, PRESENCE_CHECK);

	printf("CARD REMOVED\n");

	/* Restart discovery loop */
	NxpNci_StopDiscovery();
	NxpNci_StartDiscovery(DiscoveryTechnologies,sizeof(DiscoveryTechnologies));
}
#endif

#if defined CARDEMU_SUPPORT
#ifdef CARDEMU_RAW_EXCHANGE
void PICC_ISO14443_4_scenario (void)
{
	unsigned char OK[] = {0x90, 0x00};
	unsigned char OK1[] = {0x90, 0x01};
	unsigned char OK2[] = {0x90, 0x02};

	unsigned char Cmd[256];
	unsigned char CmdSize;
	bool lock = true;

	while (1)
	{
		if(NxpNci_CardModeReceive(Cmd, &CmdSize) == NFC_SUCCESS)
		{
		    printf("NxpNci_CardModeReceive: ");
		    for(int i=0;i<CmdSize;i++){
		        printf("%02x ",Cmd[i]);
		    }
		    printf("\n");
			if ((CmdSize >= 2) && (Cmd[0] == 0x00) && (Cmd[1] == 0xa4))
			{
				printf("Select AID received\n");
				NxpNci_CardModeSend(OK, sizeof(OK));
			}
			else if ((CmdSize >= 4) && (Cmd[0] == 0x00) && (Cmd[1] == 0xb0))
			{
	            // process read binary here
			}
			else if ((CmdSize >= 4) && (Cmd[0] == 0x00) && (Cmd[1] == 0xd0))
			{
	            // process write binary here
	            if(Cmd[3] == 0x00)
	            {
	            	lock = !lock;
	            	lock ? NxpNci_CardModeSend(OK2, sizeof(OK2)) : NxpNci_CardModeSend(OK1, sizeof(OK1));
	            }
			}
			else if ((CmdSize >= 3) && (Cmd[0] == 0x00) && (Cmd[1] == 0xfe))
			{
	            // return successful termination
				NxpNci_CardModeSend(OK, sizeof(OK));
			}
		}
		else
		{
			printf("End of transaction\n");
			return;
		}
	}
}
#endif
#endif

void task_nfc(void)
{
	NxpNci_RfIntf_t RfInterface;

#ifdef CARDEMU_SUPPORT
	/* Register NDEF message to be sent to remote reader */
	T4T_NDEF_EMU_SetMessage((unsigned char *) NDEF_MESSAGE, sizeof(NDEF_MESSAGE), *NdefPush_Cb);
#endif

#ifdef P2P_SUPPORT
	/* Register NDEF message to be sent to remote peer */
	P2P_NDEF_SetMessage((unsigned char *) NDEF_MESSAGE, sizeof(NDEF_MESSAGE), *NdefPush_Cb);
	/* Register callback for reception of NDEF message from remote peer */
	P2P_NDEF_RegisterPullCallback(*NdefPull_Cb);
#endif

#ifdef RW_SUPPORT
	/* Register callback for reception of NDEF message from remote cards */
	RW_NDEF_RegisterPullCallback(*NdefPull_Cb);
#endif

	/* Open connection to NXPNCI device */
	if (NxpNci_Connect() == NFC_ERROR) {
		printf("Error: cannot connect to NXPNCI device\n");
		return;
	}

	if (NxpNci_ConfigureSettings() == NFC_ERROR) {
		printf("Error: cannot configure NXPNCI settings\n");
		return;
	}

	if (NxpNci_ConfigureMode(mode) == NFC_ERROR)
	{
		printf("Error: cannot configure NXPNCI\n");
		return;
	}

	/* Start Discovery */
	if (NxpNci_StartDiscovery(DiscoveryTechnologies,sizeof(DiscoveryTechnologies)) != NFC_SUCCESS)
	{
		printf("Error: cannot start discovery\n");
		return;
	}

	while(1)
	{
		printf("\nWAITING FOR DEVICE DISCOVERY\n");

		/* Wait until a peer is discovered */
		NxpNci_WaitForDiscoveryNotification(&RfInterface);

#ifdef CARDEMU_SUPPORT
		/* Is activated from remote T4T ? */
		if ((RfInterface.Interface == INTF_ISODEP) && ((RfInterface.ModeTech & MODE_MASK) == MODE_LISTEN))
		{
			printf(" - LISTEN MODE: Activated from remote Reader\n");
#ifndef CARDEMU_RAW_EXCHANGE
			NxpNci_ProcessCardMode(RfInterface);
#else
			PICC_ISO14443_4_scenario();
#endif
			printf("READER DISCONNECTED\n");
		}
		else
#endif

#ifdef P2P_SUPPORT
		/* Is activated from remote T4T ? */
		if (RfInterface.Interface == INTF_NFCDEP)
		{
			if ((RfInterface.ModeTech & MODE_LISTEN) == MODE_LISTEN) printf(" - P2P TARGET MODE: Activated from remote Initiator\n");
			else printf(" - P2P INITIATOR MODE: Remote Target activated\n");

			/* Process with SNEP for NDEF exchange */
			NxpNci_ProcessP2pMode(RfInterface);

			printf("PEER LOST\n");
		}
		else
#endif
#ifdef RW_SUPPORT
		if ((RfInterface.ModeTech & MODE_MASK) == MODE_POLL)
		{
			task_nfc_reader(RfInterface);
		}
		else
#endif
		{
			printf("WRONG DISCOVERY\n");
		}
	}
}
