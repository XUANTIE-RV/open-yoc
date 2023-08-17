#include "uac_descriptor.h"

#include "io.h"
#include "usbd_core.h"
#include "usbd_audio.h"

#define AUDIO_SAMPLE_FREQ_3B(frq)  (uint8_t)(frq), (uint8_t)((frq >> 8)), (uint8_t)((frq >> 16))
#define AUDIO_SAMPLE_FREQ_4B(frq)  (uint8_t)(frq), (uint8_t)((frq >> 8)), \
                                  (uint8_t)((frq >> 16)), (uint8_t)((frq >> 24))

static struct usb_interface_association_descriptor iad_desc = {
	.bLength = sizeof iad_desc,
	.bDescriptorType = USB_DESCRIPTOR_TYPE_INTERFACE_ASSOCIATION,

	.bFirstInterface = AUDIO_FIRST_INTERFACE,
	.bInterfaceCount = AUDIO_INTERFACE_COUNT,
	.bFunctionClass = USB_DEVICE_CLASS_AUDIO,
	.bFunctionSubClass = AUDIO_CATEGORY_THEATER,
	.bFunctionProtocol = 0x00,
    .iFunction = 4,
};

/* Audio Control Interface */
static struct usb_interface_descriptor std_ac_if_desc = {
	.bLength = sizeof std_ac_if_desc,
	.bDescriptorType = USB_DESCRIPTOR_TYPE_INTERFACE,
	.bInterfaceNumber = AUDIO_FIRST_INTERFACE,
	.bAlternateSetting = 0,
	.bNumEndpoints = 0,
	.bInterfaceClass = USB_DEVICE_CLASS_AUDIO,
	.bInterfaceSubClass = AUDIO_SUBCLASS_AUDIOCONTROL,
	.bInterfaceProtocol = 0x00,
	.iInterface = 5,
};

#define UAC_AC_HEADER_DESCRIPTOR(n) \
	audio_cs_if_ac_header_descriptor_##n

#define DECLARE_UAC_AC_HEADER_DESCRIPTOR(n)			\
struct audio_cs_if_ac_header_descriptor_##n {			\
	uint8_t  bLength;						\
	uint8_t  bDescriptorType;					\
	uint8_t  bDescriptorSubtype;				\
	uint16_t bcdADC;						\
	uint16_t wTotalLength;					\
	uint8_t  bInCollection;					\
	uint8_t  baInterfaceNr[n];					\
} __PACKED

DECLARE_UAC_AC_HEADER_DESCRIPTOR(AUDIO_STREAM_INTF_COUNT);
struct UAC_AC_HEADER_DESCRIPTOR(AUDIO_STREAM_INTF_COUNT) ac_hdr_desc = {
	.bLength =	sizeof ac_hdr_desc,
	.bDescriptorType =	USB_CS_DESCRIPTOR_TYPE_INTERFACE,
	.bDescriptorSubtype =	AUDIO_CONTROL_HEADER,
	.bcdADC = cpu_to_le16(0x0100),
	.wTotalLength = cpu_to_le16(0),	/* dynamic*/
	.bInCollection = AUDIO_STREAM_INTF_COUNT,
	.baInterfaceNr = {
	/* Interface number of the AudioStream interfaces */
		[0] = 3,
		[1] = 4,
	}
};

#define UAC_FEATURE_UNIT_DESCRIPTOR(n) \
	audio_cs_if_ac_feature_unit_descriptor_##n

#define DECLARE_UAC_FEATURE_UNIT_DESCRIPTOR(ch)			\
struct audio_cs_if_ac_feature_unit_descriptor_##ch {	\
	uint8_t bLength;						\
	uint8_t bDescriptorType;					\
	uint8_t bDescriptorSubtype;				\
	uint8_t bUnitID;						\
	uint8_t bSourceID;					\
	uint8_t bControlSize;					\
	uint8_t bmaControls[ch];			\
	uint8_t iFeature;						\
} __PACKED

DECLARE_UAC_FEATURE_UNIT_DESCRIPTOR(AUDIO_CHANNEL_NUM);

#define IO_OUT_IT_ID	1
static struct audio_cs_if_ac_input_terminal_descriptor usb_out_it_desc = {
	.bLength		= sizeof usb_out_it_desc,
	.bDescriptorType	= USB_CS_DESCRIPTOR_TYPE_INTERFACE,
	.bDescriptorSubtype	= AUDIO_CONTROL_INPUT_TERMINAL,
	.bTerminalID		= IO_OUT_IT_ID,
	.wTerminalType		= cpu_to_le16(AUDIO_TERMINAL_STREAMING),
	.bAssocTerminal		= 0,
	.bNrChannels		= AUDIO_CHANNEL_NUM,
	.wChannelConfig		= cpu_to_le16(AUDIO_CHANNEL_FL),
};

#define USB_OUT_FEATURE_ID	2
static struct UAC_FEATURE_UNIT_DESCRIPTOR(AUDIO_CHANNEL_NUM) usb_out_feautre_desc = {
	.bLength = sizeof usb_out_feautre_desc,
	.bDescriptorType = USB_CS_DESCRIPTOR_TYPE_INTERFACE,
	.bDescriptorSubtype =AUDIO_CONTROL_FEATURE_UNIT,
	.bUnitID = USB_OUT_FEATURE_ID,
	.bSourceID = IO_OUT_IT_ID,
	.bControlSize = 1,
	.bmaControls = {
		[0] = 0x03,
		// [1] = 0x00,
	},
	.iFeature = 0,
};

#define USB_OUT_OT_ID	2
static struct audio_cs_if_ac_output_terminal_descriptor io_out_ot_desc = {
	.bLength =		sizeof io_out_ot_desc,
	.bDescriptorType =	USB_CS_DESCRIPTOR_TYPE_INTERFACE,
	.bDescriptorSubtype =	AUDIO_CONTROL_OUTPUT_TERMINAL,
	.bTerminalID =		USB_OUT_OT_ID,
	.wTerminalType =	cpu_to_le16(AUDIO_OUTTERM_SPEAKER),
	.bAssocTerminal =	0,
	.bSourceID =		IO_OUT_IT_ID,
};

#define USB_IN_IT_ID	3
static struct audio_cs_if_ac_input_terminal_descriptor io_in_it_desc = {
	.bLength =		sizeof io_in_it_desc,
	.bDescriptorType =	USB_CS_DESCRIPTOR_TYPE_INTERFACE,
	.bDescriptorSubtype =	AUDIO_CONTROL_INPUT_TERMINAL,
	.bTerminalID =		USB_IN_IT_ID,
	.wTerminalType =	cpu_to_le16(AUDIO_INTERM_MIC),
	.bAssocTerminal =	0,
	.bNrChannels	=	AUDIO_CHANNEL_NUM,
	.wChannelConfig =	cpu_to_le16(AUDIO_CHANNEL_FL),
};


#define USB_IN_FEATURE_ID	5
static struct UAC_FEATURE_UNIT_DESCRIPTOR(AUDIO_CHANNEL_NUM) usb_in_feautre_desc = {
	.bLength = sizeof usb_in_feautre_desc,
	.bDescriptorType = USB_CS_DESCRIPTOR_TYPE_INTERFACE,
	.bDescriptorSubtype =AUDIO_CONTROL_FEATURE_UNIT,
	.bUnitID = USB_IN_FEATURE_ID,
	.bSourceID = USB_IN_IT_ID,
	.bControlSize = 1,
	.bmaControls = {
		[0] = 0x03,
		// [1] = 0x00,
	},
	.iFeature = 0,
};

#define IO_IN_OT_ID	4
static struct audio_cs_if_ac_output_terminal_descriptor usb_in_ot_desc = {
	.bLength		= sizeof usb_in_ot_desc,
	.bDescriptorType	= USB_CS_DESCRIPTOR_TYPE_INTERFACE,
	.bDescriptorSubtype	= AUDIO_CONTROL_OUTPUT_TERMINAL,
	.bTerminalID		= IO_IN_OT_ID,
	.wTerminalType		= cpu_to_le16(AUDIO_TERMINAL_STREAMING),
	.bAssocTerminal		= 0,
	.bSourceID		= USB_IN_IT_ID,
};



static const struct usb_desc_header * const hs_audio_control_cls[] = {
	(struct usb_desc_header *)&ac_hdr_desc,
	(struct usb_desc_header *)&usb_out_it_desc,
	(struct usb_desc_header *)&usb_out_feautre_desc,
	(struct usb_desc_header *)&io_out_ot_desc,
	(struct usb_desc_header *)&io_in_it_desc,
	(struct usb_desc_header *)&usb_in_feautre_desc,
	(struct usb_desc_header *)&usb_in_ot_desc,


	NULL,
};

/* B.4.1  Standard AS Interface Descriptor */
#define AS_OUT_INTF_NR	(AUDIO_FIRST_INTERFACE + 1)
static struct usb_interface_descriptor as_out_interface_alt_0_desc = {
	.bLength =		sizeof as_out_interface_alt_0_desc,
	.bDescriptorType =	USB_DESCRIPTOR_TYPE_INTERFACE,
	.bInterfaceNumber = AS_OUT_INTF_NR,
	.bAlternateSetting =	0,
	.bNumEndpoints =	0,
	.bInterfaceClass =	USB_DEVICE_CLASS_AUDIO,
	.bInterfaceSubClass =	AUDIO_SUBCLASS_AUDIOSTREAMING,
	.bInterfaceProtocol	=	0,
	.iInterface	=	5
};

static struct usb_interface_descriptor as_out_interface_alt_1_desc = {
	.bLength =		sizeof as_out_interface_alt_1_desc,
	.bDescriptorType =	USB_DESCRIPTOR_TYPE_INTERFACE,
	.bInterfaceNumber = AS_OUT_INTF_NR,
	.bAlternateSetting =	1,
	.bNumEndpoints =	1,
	.bInterfaceClass =	USB_DEVICE_CLASS_AUDIO,
	.bInterfaceSubClass =	AUDIO_SUBCLASS_AUDIOSTREAMING,
	.bInterfaceProtocol	=	0,
	.iInterface	=	5,
};

/* B.4.2  Class-Specific AS Interface Descriptor */
static struct audio_cs_if_as_general_descriptor as_out_general_desc = {
	.bLength =		sizeof as_out_general_desc,
	.bDescriptorType =	USB_CS_DESCRIPTOR_TYPE_INTERFACE,
	.bDescriptorSubtype =	AUDIO_STREAMING_GENERAL,
	.bTerminalLink =	IO_OUT_IT_ID,
	.bDelay =		1,
	.wFormatTag =		cpu_to_le16(AUDIO_FORMAT_PCM),
};


static struct audio_cs_if_as_format_type_descriptor as_out_type_i_desc = {
	.bLength =		sizeof as_out_type_i_desc,
	.bDescriptorType =	USB_CS_DESCRIPTOR_TYPE_INTERFACE,
	.bDescriptorSubtype =	AUDIO_STREAMING_FORMAT_TYPE,
	.bFormatType =		AUDIO_FORMAT_TYPEI,
	.bNrChannels = AUDIO_CHANNEL_NUM,
	.bSubframeSize =	2,
	.bBitResolution =	16,
	.bSamFreqType =		1,
	.tSamFreq = {
		AUDIO_SAMPLE_FREQ_3B(AUDIO_FREQ)
	}
};

/* Standard ISO OUT Endpoint Descriptor */
static struct audio_ep_descriptor as_iso_out_ep_desc  = {
	.bLength =		sizeof as_iso_out_ep_desc,
	.bDescriptorType =	USB_DESCRIPTOR_TYPE_ENDPOINT,
	.bEndpointAddress =	AUDIO_OUT_EP,
	.bmAttributes =		USB_ENDPOINT_TYPE_ISOCHRONOUS,
	.wMaxPacketSize	=	cpu_to_le16(64),
	.bInterval =		EP_INTERVAL,
};

/* Class-specific AS ISO OUT Endpoint Descriptor */
static struct audio_cs_ep_ep_general_descriptor cs_as_iso_out_ep_desc = {
	.bLength =		sizeof cs_as_iso_out_ep_desc,
	.bDescriptorType =	USB_CS_DESCRIPTOR_TYPE_ENDPOINT,
	.bDescriptorSubtype =	AUDIO_ENDPOINT_GENERAL,
	.bmAttributes =		AUDIO_EP_CONTROL_SAMPLING_FEQ,
	.bLockDelayUnits =	0,
	.wLockDelay =		cpu_to_le16(0),
};


static const struct usb_desc_header * const hs_audio_out_intf[] = {
	(struct usb_desc_header *)&as_out_interface_alt_0_desc,
	(struct usb_desc_header *)&as_out_interface_alt_1_desc,
	(struct usb_desc_header *)&as_out_general_desc,
	(struct usb_desc_header *)&as_out_type_i_desc,
	(struct usb_desc_header *)&as_iso_out_ep_desc,
	(struct usb_desc_header *)&cs_as_iso_out_ep_desc,

	NULL,
};

/* B.4.1  Standard AS Interface Descriptor */
#define AS_IN_INTF_NR	(AUDIO_FIRST_INTERFACE + 2)
static struct usb_interface_descriptor as_in_interface_alt_0_desc = {
	.bLength =		sizeof as_in_interface_alt_0_desc,
	.bDescriptorType =	USB_DESCRIPTOR_TYPE_INTERFACE,
	.bInterfaceNumber = AS_IN_INTF_NR,
	.bAlternateSetting =	0,
	.bNumEndpoints =	0,
	.bInterfaceClass =	USB_DEVICE_CLASS_AUDIO,
	.bInterfaceSubClass =	AUDIO_SUBCLASS_AUDIOSTREAMING,
	.bInterfaceProtocol	=	0,
	.iInterface	=	5
};

static struct usb_interface_descriptor as_in_interface_alt_1_desc = {
	.bLength =		sizeof as_in_interface_alt_1_desc,
	.bDescriptorType =	USB_DESCRIPTOR_TYPE_INTERFACE,
	.bInterfaceNumber = AS_IN_INTF_NR,
	.bAlternateSetting =	1,
	.bNumEndpoints =	1,
	.bInterfaceClass =	USB_DEVICE_CLASS_AUDIO,
	.bInterfaceSubClass =	AUDIO_SUBCLASS_AUDIOSTREAMING,
	.bInterfaceProtocol	=	0,
	.iInterface	=	5,
};

/* B.4.2  Class-Specific AS Interface Descriptor */
static struct audio_cs_if_as_general_descriptor as_in_general_desc = {
	.bLength =		sizeof as_in_general_desc,
	.bDescriptorType =	USB_CS_DESCRIPTOR_TYPE_INTERFACE,
	.bDescriptorSubtype =	AUDIO_STREAMING_GENERAL,
	.bTerminalLink =	IO_IN_OT_ID,
	.bDelay =		1,
	.wFormatTag =		cpu_to_le16(AUDIO_FORMAT_PCM),
};


static struct audio_cs_if_as_format_type_descriptor as_in_type_i_desc = {
	.bLength =		sizeof as_in_type_i_desc,
	.bDescriptorType =	USB_CS_DESCRIPTOR_TYPE_INTERFACE,
	.bDescriptorSubtype =	AUDIO_STREAMING_FORMAT_TYPE,
	.bFormatType =		AUDIO_FORMAT_TYPEI,
	.bNrChannels = AUDIO_CHANNEL_NUM,
	.bSubframeSize =	2,
	.bBitResolution =	16,
	.bSamFreqType =		1,
	.tSamFreq = {
		AUDIO_SAMPLE_FREQ_3B(AUDIO_FREQ)
	}
};

/* Standard ISO OUT Endpoint Descriptor */
static struct audio_ep_descriptor as_iso_in_ep_desc  = {
	.bLength =		sizeof as_iso_in_ep_desc,
	.bDescriptorType =	USB_DESCRIPTOR_TYPE_ENDPOINT,
	.bEndpointAddress =	AUDIO_IN_EP,
	.bmAttributes =		USB_ENDPOINT_TYPE_ISOCHRONOUS,
	.wMaxPacketSize	=	cpu_to_le16(64),
	.bInterval =		EP_INTERVAL,
};

/* Class-specific AS ISO OUT Endpoint Descriptor */
static struct audio_cs_ep_ep_general_descriptor cs_as_iso_in_ep_desc = {
	.bLength =		sizeof cs_as_iso_in_ep_desc,
	.bDescriptorType =	USB_CS_DESCRIPTOR_TYPE_ENDPOINT,
	.bDescriptorSubtype =	AUDIO_ENDPOINT_GENERAL,
	.bmAttributes =		AUDIO_EP_CONTROL_SAMPLING_FEQ,
	.bLockDelayUnits =	0,
	.wLockDelay =		cpu_to_le16(0),
};


static const struct usb_desc_header * const hs_audio_in_intf[] = {
	(struct usb_desc_header *)&as_in_interface_alt_0_desc,
	(struct usb_desc_header *)&as_in_interface_alt_1_desc,
	(struct usb_desc_header *)&as_in_general_desc,
	(struct usb_desc_header *)&as_in_type_i_desc,
	(struct usb_desc_header *)&as_iso_in_ep_desc,
	(struct usb_desc_header *)&cs_as_iso_in_ep_desc,

	NULL,
};


static const struct usb_desc_header *hs_audio_desc[] = {
	(struct usb_desc_header *)&iad_desc,
	(struct usb_desc_header *)&std_ac_if_desc,

	NULL,
};

/* Use macro to overcome line length limitation */
#define USBDHDR(p) (struct usb_desc_header *)(p)
#define UAC_COPY_DESCRIPTOR(mem, desc) \
	do { \
		memcpy(mem, desc, (desc)->bLength); \
		mem += (desc)->bLength; \
	} while (0);

#define UAC_COPY_DESCRIPTORS(mem, src) \
	do { \
		const struct usb_desc_header * const *__src; \
		for (__src = src; *__src; ++__src) { \
			memcpy(mem, *__src, (*__src)->bLength); \
			mem += (*__src)->bLength; \
		} \
	} while (0)


uint8_t *uac_build_descriptor(uint32_t *len)
{
	uint8_t *desc = NULL;
	void *mem = NULL;
	uint32_t bytes = 0;
	uint32_t tmp_count = 0;

	for (uint32_t i = 0; hs_audio_desc[i] != NULL; i++) {
		bytes += hs_audio_desc[i]->bLength;
	}

	tmp_count = 0;
	for (uint32_t i = 0; hs_audio_control_cls[i] != NULL; i++) {
		tmp_count += hs_audio_control_cls[i]->bLength;
	}
	((struct UAC_AC_HEADER_DESCRIPTOR(AUDIO_STREAM_INTF_COUNT) *)
							hs_audio_control_cls[0])->wTotalLength = cpu_to_le16(tmp_count);
	bytes += tmp_count;

	tmp_count = 0;
	for (uint32_t i = 0; hs_audio_out_intf[i] != NULL; i++) {
		tmp_count += hs_audio_out_intf[i]->bLength;
	}
	bytes += tmp_count;

	tmp_count = 0;
	for (uint32_t i = 0; hs_audio_in_intf[i] != NULL; i++) {
		tmp_count += hs_audio_in_intf[i]->bLength;
	}
	bytes += tmp_count;
	if (len) {
		*len = bytes;
	}

	// one extra byte to hold NULL pointer
	bytes += sizeof(void *);

	desc = (uint8_t *)malloc(bytes);

	mem = desc;
	UAC_COPY_DESCRIPTORS(mem, hs_audio_desc);
	UAC_COPY_DESCRIPTORS(mem, hs_audio_control_cls);
	UAC_COPY_DESCRIPTORS(mem, hs_audio_out_intf);
	UAC_COPY_DESCRIPTORS(mem, hs_audio_in_intf);

	return desc;
}

void uac_destroy_descriptor(uint8_t *desc)
{
	if (desc) {
		free(desc);
	}
}