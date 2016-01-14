/*
 * SUMMARY:      MassBalance.c - calculate basin-wide mass balance
 * USAGE:        Part of DHSVM
 *
 * AUTHOR:       Mark Wigmosta
 * ORG:          Battelle - Pacific Northwest National Laboratory
 * E-MAIL:       ms_wigmosta@pnl.gov
 * ORIG-DATE:    Oct-96
 * DESCRIPTION:  Calculate water mass balance errors
 *               
 * DESCRIP-END.
 * FUNCTIONS:    MassBalance()
 * COMMENTS:
 * Modification made on 2012/12/31
 * $Id: MassBalance.c, v 4.0 Ning Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include "settings.h"
#include "data.h"
#include "DHSVMerror.h"
#include "functions.h"
#include "constants.h"
#include "Calendar.h"

/*****************************************************************************
  Aggregate()
  
  Calculate the average values for the different fluxes and state variables
  over the basin.  
  In the current implementation the local radiation
  elements are not stored for the entire area.  Therefore these components
  are aggregated in AggregateRadiation() inside MassEnergyBalance().

  The aggregated values are set to zero in the function RestAggregate,
  which is executed at the beginning of each time step.
*****************************************************************************/
void MassBalance(DATE *Current, FILES *Out, AGGREGATED *Total, WATERBALANCE *Mass)
{
  float NewWaterStorage;	/* water storage at the end of the time step */
  float Output;			/* total water flux leaving the basin;  */
  float Input;
  float MassError;		/* mass balance error m  */

  NewWaterStorage = Total->Soil.IExcess + Total->Road.IExcess + 
    Total->CanopyWater + Total->SoilWater +
    Total->Snow.Swq + Total->Soil.SatFlow + Total->Soil.DetentionStorage;

  Output = Total->ChannelInt + Total->RoadInt + Total->Evap.ETot;
  Input = Total->Precip.Precip + Total->Snow.VaporMassFlux +
    Total->Snow.CanopyVaporMassFlux + Total->CulvertReturnFlow;

  MassError = (NewWaterStorage - Mass->OldWaterStorage) + Output -
    Total->Precip.Precip - Total->Snow.VaporMassFlux -
    Total->Snow.CanopyVaporMassFlux - Total->CulvertReturnFlow;

  /* update */
  Mass->OldWaterStorage = NewWaterStorage;
  Mass->CumPrecipIn += Total->Precip.Precip;
  Mass->CumIExcess += Total->Soil.IExcess;
  Mass->CumChannelInt += Total->ChannelInt;
  Mass->CumRoadInt += Total->RoadInt;
  Mass->CumET += Total->Evap.ETot;
  Mass->CumSnowVaporFlux += Total->Snow.VaporMassFlux +
    Total->Snow.CanopyVaporMassFlux;
  Mass->CumCulvertReturnFlow += Total->CulvertReturnFlow;
  Mass->CumCulvertToChannel += Total->CulvertToChannel;
  Mass->CumRunoffToChannel += Total->RunoffToChannel;
  
  PrintDate(Current, Out->FilePtr);
  fprintf(Out->FilePtr, " %7.4f  %7.4f  %6.3f  %8.4f  %.2e  \
%.2e  %5.2f  %5.2f  %7.4f  %7.4f  %7.4f  %6.3f  %.2e  %5.2f  %.2e  %7.3f \n",
	  Total->Soil.IExcess, Total->CanopyWater, Total->SoilWater, 
	  Total->Snow.Swq, Total->Soil.SatFlow, 
	  Total->ChannelInt,  Total->RoadInt, Total->CulvertReturnFlow, Total->Evap.ETot,  
	  Total->Precip.Precip, Total->Snow.VaporMassFlux, 
	  Total->Snow.CanopyVaporMassFlux, Mass->OldWaterStorage, Total->CulvertToChannel,
	  Total->RunoffToChannel, MassError);
}