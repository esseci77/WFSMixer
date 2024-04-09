#include "PluginProcessor.h"
#include "PluginEditor.h"

int WFSRendererAudioProcessor::getNumParameters()
{
    
    return NumParameters;
}

float WFSRendererAudioProcessor::getParameter (int index)
{
    
    switch (index) {
        case 0:
            return x_param;
            break;
            
        case 1:
            return y_param;
            break;
            
            
            // advanced ctl
    /*    case XSetParam:
            return x_set_param;
            break;
      */
        case 2:
            return x_mv_param;
            break;
            
        /*
        case YSetParam:
            return y_set_param;
            break;
          */
        case 3:
            return y_mv_param;
            break;
            
        case 4:
            return speed_param;
            break;
        
    /*
        case PA_delay1:
            return pA_delay1;
            break;
        case PB_delay2:
            return pB_delay2;
            break;
        case T1_startIpol:
            return t1_startIpol;
            break;
        case GA_startIpol:
            return gA_startIpol;
            break;
        case GB_startIpol:
            return gB_startIpol;
            break;
        case T1_endIpol:
            return t1_endIpol;
            break;
        case GA_endIpol:
            return gA_endIpol;
            break;
        case GB_endIpol:
            return gB_endIpol;
            break;
        case Dt_smp:
            return dt_smp;
            break;
        case DgA_smp:
            return dgA_smp;
            break;
        case DgB_smp:
            return dgB_smp;
            break;
        case T1_decimal:
            return t1_decimal;
            break;
        case T2_decimal_recipr:
            return t2_decimal_recipr;
            break;
        case K_initial:
            return k_initial;
            break;
        case A_outp:
            return a_outp;
            break;
        case B_outp:
            return b_outp;
            break;
        case DtTot:
            return dtTot;
            break;*/
        default:
            return 0.f;
    }
    
}

void WFSRendererAudioProcessor::setParameter (int index, float newValue)
{
    
//    float tempvalue = 0.0f;
    
    switch (index) {
        case 0:
            x_param=newValue;
//            calcX();
            
            break;
            
        case 1:
            y_param=newValue;
//            for (int i=0; i < AmbiEnc.size(); i++) {
//                AmbiEnc.getUnchecked(i)->y=y_param;
//            }
            break;
            
        case 2:
            x_mv_param = newValue;
            break;
            
            
            
        case 3:
            y_mv_param = newValue;
            break;
            
        case 4:
            speed_param = newValue;
            break;
       /* case PA_delay1:
            pA_delay1 = newValue;
            break;
            
        case PB_delay2:
            pB_delay2 = newValue;
            break;
            
        case T1_startIpol:
            t1_startIpol = newValue;
            break;
            
        case GA_startIpol:
            gA_startIpol = newValue;
            break;
            
        case GB_startIpol:
            gB_startIpol = newValue;
            break;
            
        case T1_endIpol:
            t1_endIpol = newValue;
            break;
            
        case GA_endIpol:
            gA_endIpol = newValue;
            break;
            
        case GB_endIpol:
            gB_endIpol = newValue;
            break;
            
        case Dt_smp:
            dt_smp = newValue;
            break;
            
        case DgA_smp:
            dgA_smp = newValue;
            break;
            
        case DgB_smp:
            dgB_smp = newValue;
            break;
            
        case T1_decimal:
            t1_decimal = newValue;
            break;
            
        case T2_decimal_recipr:
            t2_decimal_recipr = newValue;
            break;
            
        case K_initial:
            k_initial = newValue;
            break;
            
        case A_outp:
            a_outp = newValue;
            break;
            
        case B_outp:
            b_outp = newValue;
            break;
            
        case DtTot:
            dtTot = newValue;
            break;
            */
        default:
            break;
            
    }
    
    sendChangeMessage(); // send message to gui!
}

// calculate the individual x for all sources
void WFSRendererAudioProcessor::calcX()
{
//    if (AmbiEnc.size() == 1) {
//        AmbiEnc.getFirst()->x = x_param;
//    }
//    else if (AmbiEnc.size() > 1)
//    {
//        for (int i=0; i < AmbiEnc.size(); i++) {
//            
//            float angle = x_param - width_param / 2.f + i * width_param / (AmbiEnc.size()-1);
//            
//            // angle = fmodf(angle, 1.f);
//            if (angle < 0.f)
//                angle = 1.f + angle;
//            
//            if (angle > 1.f) {
//                angle -= 1.f;
//            }
//            
//            //std::cout << "source # " << i << " angle: " << (angle-0.5f)*360 << std::endl;
//            
//            AmbiEnc.getUnchecked(i)->x = angle;
//        }
//        
//    }
//    
}



float WFSRendererAudioProcessor::automVol(float db){
    float volu = 0.f;
    volu = pow(10.0f, db * 0.1f);
    if(db <= 0.f){
        volu *= 0.5f;
    }else{
        volu -= 1.f;
        //volu *= 0.166666666666L;
        volu /= 6.f;
        volu += 0.5f;
    }
    return volu;
}

float WFSRendererAudioProcessor::slideVol(float volu){
    float voll = volu;
    float db = 0.f;
    if(volu <= 0.5f){
        voll *= 2.f;
    }else{
        voll -= 0.5f;
        voll *= 6.f;
        voll += 1.f;
    }
    db = log10(voll)*10.f;
    
    return db;
}


const String WFSRendererAudioProcessor::getParameterName (int index)
{
    switch (index) {
        case 0:
            return "X";
            break;
            
        case 1:
            return "Y";
            break;
            
        case 3:
            return "MoveX";
            break;
            
    /*
        case YSetParam:
            return "SetY";
            break;
      */
            
        case 4:
            return "MoveY";
            break;
            
        case 5:
            return "MoveSpeed";
            break;
            
       /* case PA_delay1:
            return "pA_delay1";
            break;
        case PB_delay2:
            return "pB_delay2";
            break;
        case T1_startIpol:
            return "t1_startIpol";
            break;
        case GA_startIpol:
            return "gA_startIpol";
            break;
        case GB_startIpol:
            return "gB_startIpol";
            break;
        case T1_endIpol:
            return "t1_endIpol";
            break;
        case GA_endIpol:
            return "gA_endIpol";
            break;
        case GB_endIpol:
            return "gB_endIpol";
            break;
        case Dt_smp:
            return "dt_smp";
            break;
        case DgA_smp:
            return "dgA_smp";
            break;
        case DgB_smp:
            return "dgB_smp";
            break;
        case T1_decimal:
            return "t1_decimal";
            break;
        case T2_decimal_recipr:
            return "t2_decimal_recipr";
            break;
        case K_initial:
            return "k_initial";
            break;
        case A_outp:
            return "a_outp";
            break;
        case B_outp:
            return "b_outp";
            break;
        case DtTot:
            return "dtTot";
            break;*/
        default:
            return "";
    }
	
}

const String WFSRendererAudioProcessor::getParameterText (int index)
{
    String text;
    
    switch (index) {
        case 0:
//            [gc]
//            text << String((x_param - 0.5f) * 360).substring(0, 5) << " deg";
            text << String((x_param - 0.5f) * layout->maxdist()).substring(0, 5) << " m";
            break;
            
        case 1:
//            [gc]
//            text << String((y_param - 0.5f) * 360).substring(0, 5) << " deg";
            text << String((y_param- 0.5f) * layout->maxdist()).substring(0, 5) << " m";
            break;
            
            
        case 2:
			if (x_mv_param <= 0.48f)
			{
                text << String(powf(speed_param*360.f, (0.45f - x_mv_param)*2.22222f)).substring(0, 5) << " m/sec"; // from 0->90deg/sec
			} else if (x_mv_param >= 0.52f) {
                text << String(powf(speed_param*360.f, (x_mv_param - 0.55f)*2.22222f)).substring(0, 5) << " m/sec";
			} else {
				text << "do not rotate";
			}
			break;
          
       /*
        case YSetParam:
//          [gc]
//          text << String((y_set_param - 0.5f) * 360).substring(0, 5) << " deg";
            text << String((y_set_param- 0.5f) * layout->maxdist()).substring(0, 5) << " m";
            break;
         */
        
            
       case 3:
			if (y_mv_param <= 0.48f)
			{
                text << String(powf(speed_param*360.f, (0.45f - y_mv_param)*2.22222f)).substring(0, 5) << " m/sec"; // from 0->90deg/sec
			} else if (y_mv_param >= 0.52f) {
                text << String(powf(speed_param*360.f, (y_mv_param - 0.55f)*2.22222f)).substring(0, 5) << " m/sec";
			} else {
				text << "do not rotate";
			}
			break;
            
        case 4:
            text << String((speed_param) * 360).substring(0, 5) << " m";
            break;
            
     /*
        case PA_delay1:
            text <<  pA_delay1;
            break;
        case PB_delay2:
            text <<  pB_delay2;
            break;
        case T1_startIpol:
            text <<  t1_startIpol;
            break;
        case GA_startIpol:
            text <<  gA_startIpol;
            break;
        case GB_startIpol:
            text <<  gB_startIpol;
            break;
        case T1_endIpol:
            text <<  t1_endIpol;
            break;
        case GA_endIpol:
            text <<  gA_endIpol;
            break;
        case GB_endIpol:
            text <<  gB_endIpol;
            break;
        case Dt_smp:
            text <<  dt_smp;
            break;
        case DgA_smp:
            text <<  dgA_smp;
            break;
        case DgB_smp:
            text <<  dgB_smp;
            break;
        case T1_decimal:
            text <<  t1_decimal;
            break;
        case T2_decimal_recipr:
            text <<  t2_decimal_recipr;
            break;
        case K_initial:
            text <<  k_initial;
            break;
        case A_outp:
            text <<  a_outp;
            break;
        case B_outp:
            text <<  b_outp;
            break;
        case DtTot:
            text <<  dtTot;
            break;*/
        default:
            break;
    }
    
	return text;
}

const String WFSRendererAudioProcessor::getParameterLabel(int index)
{
    String text;
    
    switch (index) {
        case 0:
            text << "meter";
            break;
            
        case 1:
            text << "meter";
            break;
            
       case 2:
            text << "meter/sec";
        break;
            
        /*
        case YSetParam:
            text << "meter";
            break;
          */  
      
            
        case 3:
            text << "meter/sec";
            break;
            
        case 4:
            text << "meter/sec";
            break;
        
        /*case PA_delay1:
            text <<  "db";
            break;
        case PB_delay2:
            text <<  "db";
            break;
        case T1_startIpol:
            text <<  "ms";
            break;
        case GA_startIpol:
            text <<  "db";
            break;
        case GB_startIpol:
            text <<  "db";
            break;
        case T1_endIpol:
            text <<  "ms";
            break;
        case GA_endIpol:
            text <<  "db";
            break;
        case GB_endIpol:
            text <<  "db";
            break;
        case Dt_smp:
            text <<  "ms";
            break;
        case DgA_smp:
            text <<  "db";
            break;
        case DgB_smp:
            text <<  "db";
            break;
        case T1_decimal:
            text <<  "ms";
            break;
        case T2_decimal_recipr:
            text <<  "ms";
            break;
        case K_initial:
            text <<"";
            break;
        case A_outp:
            text <<"";
            break;
        case B_outp:
            text <<"";
            break;
        case DtTot:
            text <<  "ms";
            break;*/
        default:
            break;
    }
    
	return text;
}

#if WITH_ADVANCED_CONTROL
void WFSRendererAudioProcessor::calcNewParameters(double SampleRate, int BufferLength)
{
    // calculate moving parameters
    
    double factor = (double)BufferLength/SampleRate;
    float speed_fact2 = (float)factor * 0.002777777f; // (1/360)
    
    float deg_sec = speed_param*360.f;
    
    float newval = 0.f;
    
    if ((x_mv_param < 0.48f) || (x_mv_param > 0.52f))
	{
        if (x_mv_param < 0.48f)
            newval = x_param - powf(deg_sec, (0.48f - x_mv_param)*2.0833333f) * speed_fact2;
		
        if (x_mv_param > 0.52f)
            newval = x_param + powf(deg_sec, (x_mv_param - 0.52f)*2.0833333f) * speed_fact2;
		
		if (newval < 0.f)
			newval = 1.f;
		if (newval > 1.f)
			newval = 0.f;
		setParameterNotifyingHost(0, newval);
	}
    
    if ((y_mv_param <= 0.45f) || (y_mv_param >= 0.55f))
	{
        if (y_mv_param <= 0.45f)
            newval = y_param - powf(deg_sec, (0.45f - y_mv_param)*2.22222f) * speed_fact2;
		
        if (y_mv_param >= 0.55f)
            newval = y_param + powf(deg_sec, (y_mv_param - 0.55f)*2.22222f) * speed_fact2;
		
		if (newval < 0.f)
			newval = 1.f;
		if (newval > 1.f)
			newval = 0.f;
		setParameterNotifyingHost(1, newval);
	}
    
}
#endif