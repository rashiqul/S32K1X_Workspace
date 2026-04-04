#ifndef DIO_H
#define DIO_H

#include "Std_Types.h"

typedef uint16 Dio_ChannelType;

#define DioConf_DioChannel_DioChannel_0 ((Dio_ChannelType)0U)

void Dio_FlipChannel(Dio_ChannelType channelId);

#endif /* DIO_H */
