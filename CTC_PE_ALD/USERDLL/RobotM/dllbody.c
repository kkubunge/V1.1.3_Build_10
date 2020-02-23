#include <stdio.h>
#include <process.h>
#include <time.h>
#include <windows.h>
#include <wingdi.h>
#include <winbase.h>
#include <windowsx.h>
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
double memsin[] = {
	0.000000 ,	0.001745 ,	0.003491 ,	0.005236 ,	0.006981 ,	0.008727 ,	0.010472 ,	0.012217 ,	0.013962 ,	0.015707 ,
	0.017452 ,	0.019197 ,	0.020942 ,	0.022687 ,	0.024432 ,	0.026177 ,	0.027922 ,	0.029666 ,	0.031411 ,	0.033155 ,
	0.034899 ,	0.036644 ,	0.038388 ,	0.040132 ,	0.041876 ,	0.043619 ,	0.045363 ,	0.047106 ,	0.048850 ,	0.050593 ,
	0.052336 ,	0.054079 ,	0.055822 ,	0.057564 ,	0.059306 ,	0.061049 ,	0.062791 ,	0.064532 ,	0.066274 ,	0.068015 ,
	0.069756 ,	0.071497 ,	0.073238 ,	0.074979 ,	0.076719 ,	0.078459 ,	0.080199 ,	0.081939 ,	0.083678 ,	0.085417 ,
	0.087156 ,	0.088894 ,	0.090633 ,	0.092371 ,	0.094108 ,	0.095846 ,	0.097583 ,	0.099320 ,	0.101056 ,	0.102793 ,
	0.104528 ,	0.106264 ,	0.107999 ,	0.109734 ,	0.111469 ,	0.113203 ,	0.114937 ,	0.116671 ,	0.118404 ,	0.120137 ,
	0.121869 ,	0.123601 ,	0.125333 ,	0.127065 ,	0.128796 ,	0.130526 ,	0.132256 ,	0.133986 ,	0.135716 ,	0.137445 ,
	0.139173 ,	0.140901 ,	0.142629 ,	0.144356 ,	0.146083 ,	0.147809 ,	0.149535 ,	0.151261 ,	0.152986 ,	0.154710 ,
	0.156434 ,	0.158158 ,	0.159881 ,	0.161604 ,	0.163326 ,	0.165048 ,	0.166769 ,	0.168489 ,	0.170209 ,	0.171929 ,
	0.173648 ,	0.175367 ,	0.177085 ,	0.178802 ,	0.180519 ,	0.182236 ,	0.183951 ,	0.185667 ,	0.187381 ,	0.189095 ,
	0.190809 ,	0.192522 ,	0.194234 ,	0.195946 ,	0.197657 ,	0.199368 ,	0.201078 ,	0.202787 ,	0.204496 ,	0.206204 ,
	0.207912 ,	0.209619 ,	0.211325 ,	0.213030 ,	0.214735 ,	0.216440 ,	0.218143 ,	0.219846 ,	0.221548 ,	0.223250 ,
	0.224951 ,	0.226651 ,	0.228351 ,	0.230050 ,	0.231748 ,	0.233445 ,	0.235142 ,	0.236838 ,	0.238533 ,	0.240228 ,
	0.241922 ,	0.243615 ,	0.245307 ,	0.246999 ,	0.248690 ,	0.250380 ,	0.252069 ,	0.253758 ,	0.255446 ,	0.257133 ,
	0.258819 ,	0.260505 ,	0.262189 ,	0.263873 ,	0.265556 ,	0.267238 ,	0.268920 ,	0.270600 ,	0.272280 ,	0.273959 ,
	0.275637 ,	0.277315 ,	0.278991 ,	0.280667 ,	0.282341 ,	0.284015 ,	0.285688 ,	0.287361 ,	0.289032 ,	0.290702 ,
	0.292372 ,	0.294040 ,	0.295708 ,	0.297375 ,	0.299041 ,	0.300706 ,	0.302370 ,	0.304033 ,	0.305695 ,	0.307357 ,
	0.309017 ,	0.310676 ,	0.312335 ,	0.313992 ,	0.315649 ,	0.317305 ,	0.318959 ,	0.320613 ,	0.322266 ,	0.323917 ,
	0.325568 ,	0.327218 ,	0.328867 ,	0.330514 ,	0.332161 ,	0.333807 ,	0.335452 ,	0.337095 ,	0.338738 ,	0.340380 ,
	0.342020 ,	0.343660 ,	0.345298 ,	0.346936 ,	0.348572 ,	0.350207 ,	0.351842 ,	0.353475 ,	0.355107 ,	0.356738 ,
	0.358368 ,	0.359997 ,	0.361625 ,	0.363251 ,	0.364877 ,	0.366501 ,	0.368125 ,	0.369747 ,	0.371368 ,	0.372988 ,
	0.374607 ,	0.376224 ,	0.377841 ,	0.379456 ,	0.381070 ,	0.382683 ,	0.384295 ,	0.385906 ,	0.387516 ,	0.389124 ,
	0.390731 ,	0.392337 ,	0.393942 ,	0.395546 ,	0.397148 ,	0.398749 ,	0.400349 ,	0.401948 ,	0.403545 ,	0.405142 ,
	0.406737 ,	0.408330 ,	0.409923 ,	0.411514 ,	0.413104 ,	0.414693 ,	0.416281 ,	0.417867 ,	0.419452 ,	0.421036 ,
	0.422618 ,	0.424199 ,	0.425779 ,	0.427358 ,	0.428935 ,	0.430511 ,	0.432086 ,	0.433659 ,	0.435231 ,	0.436802 ,
	0.438371 ,	0.439939 ,	0.441506 ,	0.443071 ,	0.444635 ,	0.446198 ,	0.447759 ,	0.449319 ,	0.450878 ,	0.452435 ,
	0.453990 ,	0.455545 ,	0.457098 ,	0.458650 ,	0.460200 ,	0.461749 ,	0.463296 ,	0.464842 ,	0.466387 ,	0.467930 ,
	0.469472 ,	0.471012 ,	0.472551 ,	0.474088 ,	0.475624 ,	0.477159 ,	0.478692 ,	0.480223 ,	0.481754 ,	0.483282 ,
	0.484810 ,	0.486335 ,	0.487860 ,	0.489382 ,	0.490904 ,	0.492424 ,	0.493942 ,	0.495459 ,	0.496974 ,	0.498488 ,
	0.500000 ,	0.501511 ,	0.503020 ,	0.504528 ,	0.506034 ,	0.507538 ,	0.509041 ,	0.510543 ,	0.512043 ,	0.513541 ,
	0.515038 ,	0.516533 ,	0.518027 ,	0.519519 ,	0.521010 ,	0.522499 ,	0.523986 ,	0.525472 ,	0.526956 ,	0.528438 ,
	0.529919 ,	0.531399 ,	0.532876 ,	0.534352 ,	0.535827 ,	0.537300 ,	0.538771 ,	0.540240 ,	0.541708 ,	0.543174 ,
	0.544639 ,	0.546102 ,	0.547563 ,	0.549023 ,	0.550481 ,	0.551937 ,	0.553392 ,	0.554844 ,	0.556296 ,	0.557745 ,
	0.559193 ,	0.560639 ,	0.562083 ,	0.563526 ,	0.564967 ,	0.566406 ,	0.567844 ,	0.569280 ,	0.570714 ,	0.572146 ,
	0.573576 ,	0.575005 ,	0.576432 ,	0.577858 ,	0.579281 ,	0.580703 ,	0.582123 ,	0.583541 ,	0.584958 ,	0.586372 ,
	0.587785 ,	0.589196 ,	0.590606 ,	0.592013 ,	0.593419 ,	0.594823 ,	0.596225 ,	0.597625 ,	0.599024 ,	0.600420 ,
	0.601815 ,	0.603208 ,	0.604599 ,	0.605988 ,	0.607376 ,	0.608761 ,	0.610145 ,	0.611527 ,	0.612907 ,	0.614285 ,
	0.615661 ,	0.617036 ,	0.618408 ,	0.619779 ,	0.621148 ,	0.622515 ,	0.623880 ,	0.625243 ,	0.626604 ,	0.627963 ,
	0.629320 ,	0.630676 ,	0.632029 ,	0.633381 ,	0.634731 ,	0.636078 ,	0.637424 ,	0.638768 ,	0.640110 ,	0.641450 ,
	0.642788 ,	0.644124 ,	0.645458 ,	0.646790 ,	0.648120 ,	0.649448 ,	0.650774 ,	0.652098 ,	0.653421 ,	0.654741 ,
	0.656059 ,	0.657375 ,	0.658689 ,	0.660002 ,	0.661312 ,	0.662620 ,	0.663926 ,	0.665230 ,	0.666532 ,	0.667833 ,
	0.669131 ,	0.670427 ,	0.671721 ,	0.673013 ,	0.674302 ,	0.675590 ,	0.676876 ,	0.678160 ,	0.679441 ,	0.680721 ,
	0.681998 ,	0.683274 ,	0.684547 ,	0.685818 ,	0.687088 ,	0.688355 ,	0.689620 ,	0.690882 ,	0.692143 ,	0.693402 ,
	0.694658 ,	0.695913 ,	0.697165 ,	0.698415 ,	0.699663 ,	0.700909 ,	0.702153 ,	0.703395 ,	0.704634 ,	0.705872 ,
	0.707107 ,	0.708340 ,	0.709571 ,	0.710799 ,	0.712026 ,	0.713250 ,	0.714473 ,	0.715693 ,	0.716911 ,	0.718126 ,
	0.719340 ,	0.720551 ,	0.721760 ,	0.722967 ,	0.724172 ,	0.725374 ,	0.726575 ,	0.727773 ,	0.728969 ,	0.730162 ,
	0.731354 ,	0.732543 ,	0.733730 ,	0.734915 ,	0.736097 ,	0.737277 ,	0.738455 ,	0.739631 ,	0.740805 ,	0.741976 ,
	0.743145 ,	0.744312 ,	0.745476 ,	0.746638 ,	0.747798 ,	0.748956 ,	0.750111 ,	0.751264 ,	0.752415 ,	0.753563 ,
	0.754710 ,	0.755853 ,	0.756995 ,	0.758134 ,	0.759271 ,	0.760406 ,	0.761538 ,	0.762668 ,	0.763796 ,	0.764921 ,
	0.766044 ,	0.767165 ,	0.768284 ,	0.769400 ,	0.770513 ,	0.771625 ,	0.772734 ,	0.773840 ,	0.774944 ,	0.776046 ,
	0.777146 ,	0.778243 ,	0.779338 ,	0.780430 ,	0.781520 ,	0.782608 ,	0.783693 ,	0.784776 ,	0.785857 ,	0.786935 ,
	0.788011 ,	0.789084 ,	0.790155 ,	0.791224 ,	0.792290 ,	0.793353 ,	0.794415 ,	0.795473 ,	0.796530 ,	0.797584 ,
	0.798636 ,	0.799685 ,	0.800731 ,	0.801776 ,	0.802817 ,	0.803857 ,	0.804894 ,	0.805928 ,	0.806960 ,	0.807990 ,
	0.809017 ,	0.810042 ,	0.811064 ,	0.812084 ,	0.813101 ,	0.814116 ,	0.815128 ,	0.816138 ,	0.817145 ,	0.818150 ,
	0.819152 ,	0.820152 ,	0.821149 ,	0.822144 ,	0.823136 ,	0.824126 ,	0.825113 ,	0.826098 ,	0.827081 ,	0.828060 ,
	0.829038 ,	0.830012 ,	0.830984 ,	0.831954 ,	0.832921 ,	0.833886 ,	0.834848 ,	0.835807 ,	0.836764 ,	0.837719 ,
	0.838671 ,	0.839620 ,	0.840567 ,	0.841511 ,	0.842452 ,	0.843391 ,	0.844328 ,	0.845262 ,	0.846193 ,	0.847122 ,
	0.848048 ,	0.848972 ,	0.849893 ,	0.850811 ,	0.851727 ,	0.852640 ,	0.853551 ,	0.854459 ,	0.855364 ,	0.856267 ,
	0.857167 ,	0.858065 ,	0.858960 ,	0.859852 ,	0.860742 ,	0.861629 ,	0.862514 ,	0.863396 ,	0.864275 ,	0.865151 ,
	0.866025 ,	0.866897 ,	0.867765 ,	0.868632 ,	0.869495 ,	0.870356 ,	0.871214 ,	0.872069 ,	0.872922 ,	0.873772 ,
	0.874620 ,	0.875465 ,	0.876307 ,	0.877146 ,	0.877983 ,	0.878817 ,	0.879649 ,	0.880477 ,	0.881303 ,	0.882127 ,
	0.882948 ,	0.883766 ,	0.884581 ,	0.885394 ,	0.886204 ,	0.887011 ,	0.887815 ,	0.888617 ,	0.889416 ,	0.890213 ,
	0.891007 ,	0.891798 ,	0.892586 ,	0.893371 ,	0.894154 ,	0.894934 ,	0.895712 ,	0.896486 ,	0.897258 ,	0.898028 ,
	0.898794 ,	0.899558 ,	0.900319 ,	0.901077 ,	0.901833 ,	0.902585 ,	0.903335 ,	0.904083 ,	0.904827 ,	0.905569 ,
	0.906308 ,	0.907044 ,	0.907777 ,	0.908508 ,	0.909236 ,	0.909961 ,	0.910684 ,	0.911403 ,	0.912120 ,	0.912834 ,
	0.913545 ,	0.914254 ,	0.914960 ,	0.915663 ,	0.916363 ,	0.917060 ,	0.917755 ,	0.918446 ,	0.919135 ,	0.919821 ,
	0.920505 ,	0.921185 ,	0.921863 ,	0.922538 ,	0.923210 ,	0.923880 ,	0.924546 ,	0.925210 ,	0.925871 ,	0.926529 ,
	0.927184 ,	0.927836 ,	0.928486 ,	0.929133 ,	0.929776 ,	0.930418 ,	0.931056 ,	0.931691 ,	0.932324 ,	0.932954 ,
	0.933580 ,	0.934204 ,	0.934826 ,	0.935444 ,	0.936060 ,	0.936672 ,	0.937282 ,	0.937889 ,	0.938493 ,	0.939094 ,
	0.939693 ,	0.940288 ,	0.940881 ,	0.941471 ,	0.942057 ,	0.942641 ,	0.943223 ,	0.943801 ,	0.944376 ,	0.944949 ,
	0.945519 ,	0.946085 ,	0.946649 ,	0.947210 ,	0.947768 ,	0.948324 ,	0.948876 ,	0.949425 ,	0.949972 ,	0.950516 ,
	0.951057 ,	0.951594 ,	0.952129 ,	0.952661 ,	0.953191 ,	0.953717 ,	0.954240 ,	0.954761 ,	0.955278 ,	0.955793 ,
	0.956305 ,	0.956814 ,	0.957319 ,	0.957822 ,	0.958323 ,	0.958820 ,	0.959314 ,	0.959805 ,	0.960294 ,	0.960779 ,
	0.961262 ,	0.961741 ,	0.962218 ,	0.962692 ,	0.963163 ,	0.963630 ,	0.964095 ,	0.964557 ,	0.965016 ,	0.965473 ,
	0.965926 ,	0.966376 ,	0.966823 ,	0.967268 ,	0.967709 ,	0.968148 ,	0.968583 ,	0.969016 ,	0.969445 ,	0.969872 ,
	0.970296 ,	0.970716 ,	0.971134 ,	0.971549 ,	0.971961 ,	0.972370 ,	0.972776 ,	0.973179 ,	0.973579 ,	0.973976 ,
	0.974370 ,	0.974761 ,	0.975149 ,	0.975535 ,	0.975917 ,	0.976296 ,	0.976672 ,	0.977046 ,	0.977416 ,	0.977783 ,
	0.978148 ,	0.978509 ,	0.978867 ,	0.979223 ,	0.979575 ,	0.979925 ,	0.980271 ,	0.980615 ,	0.980955 ,	0.981293 ,
	0.981627 ,	0.981959 ,	0.982287 ,	0.982613 ,	0.982935 ,	0.983255 ,	0.983571 ,	0.983885 ,	0.984196 ,	0.984503 ,
	0.984808 ,	0.985109 ,	0.985408 ,	0.985703 ,	0.985996 ,	0.986286 ,	0.986572 ,	0.986856 ,	0.987136 ,	0.987414 ,
	0.987688 ,	0.987960 ,	0.988228 ,	0.988494 ,	0.988756 ,	0.989016 ,	0.989272 ,	0.989526 ,	0.989776 ,	0.990024 ,
	0.990268 ,	0.990509 ,	0.990748 ,	0.990983 ,	0.991216 ,	0.991445 ,	0.991671 ,	0.991894 ,	0.992115 ,	0.992332 ,
	0.992546 ,	0.992757 ,	0.992966 ,	0.993171 ,	0.993373 ,	0.993572 ,	0.993768 ,	0.993961 ,	0.994151 ,	0.994338 ,
	0.994522 ,	0.994703 ,	0.994881 ,	0.995056 ,	0.995227 ,	0.995396 ,	0.995562 ,	0.995725 ,	0.995884 ,	0.996041 ,
	0.996195 ,	0.996345 ,	0.996493 ,	0.996637 ,	0.996779 ,	0.996917 ,	0.997053 ,	0.997185 ,	0.997314 ,	0.997441 ,
	0.997564 ,	0.997684 ,	0.997801 ,	0.997916 ,	0.998027 ,	0.998135 ,	0.998240 ,	0.998342 ,	0.998441 ,	0.998537 ,
	0.998630 ,	0.998719 ,	0.998806 ,	0.998890 ,	0.998971 ,	0.999048 ,	0.999123 ,	0.999194 ,	0.999263 ,	0.999328 ,
	0.999391 ,	0.999450 ,	0.999507 ,	0.999560 ,	0.999610 ,	0.999657 ,	0.999701 ,	0.999743 ,	0.999781 ,	0.999816 ,
	0.999848 ,	0.999877 ,	0.999903 ,	0.999925 ,	0.999945 ,	0.999962 ,	0.999976 ,	0.999986 ,	0.999994 ,	0.999998 ,
	1.000000
};
//----------------------------------------------------------------------------------
//-------------------------------------------------------------------------
//void rotate( double x , double y , double cx , double cy , double theta , double *x2 , double *y2 ) {
////	#define	PIE_VALUE	3.14159265/180
//	#define	PIE_VALUE	0.0174532925
//	double	PIE_VALUE2	= PIE_VALUE * theta;
//	*x2 = (x-cx) * cos( PIE_VALUE2 ) + (y-cy) * sin( PIE_VALUE2 ) + cx;
//	*y2 = (y-cy) * cos( PIE_VALUE2 ) - (x-cx) * sin( PIE_VALUE2 ) + cy;
//}
//============================================
// Change 2002.03.25 // for fast
//============================================
void rotate( double x , double y , double cx , double cy , double theta , double *x2 , double *y2 ) {
	int ith;
	ith = (int) ( theta * (double) 10 );
	if ( ( ith > 3600 ) || ( ith < -3600 ) ) ith = ith % 3600;
	if      ( ( ith >=    0 ) && ( ith <=  900 ) ) {
		*x2 = (x-cx) * memsin[ 900 - ith ] + (y-cy) * memsin[ ith ] + cx;
		*y2 = (y-cy) * memsin[ 900 - ith ] - (x-cx) * memsin[ ith ] + cy;
	}
	else if ( ( ith >=  900 ) && ( ith <= 1800 ) ) {
		*x2 = (x-cx) * - memsin[ ith - 900 ] + (y-cy) * memsin[ 1800 - ith ] + cx;
		*y2 = (y-cy) * - memsin[ ith - 900 ] - (x-cx) * memsin[ 1800 - ith ] + cy;
	}
	else if ( ( ith >= 1800 ) && ( ith <= 2700 ) ) {
		*x2 = (x-cx) * - memsin[ 2700 - ith ] + (y-cy) * - memsin[ ith - 1800 ] + cx;
		*y2 = (y-cy) * - memsin[ 2700 - ith ] - (x-cx) * - memsin[ ith - 1800 ] + cy;
	}
	else if ( ( ith >= 2700 ) && ( ith <= 3600 ) ) {
		*x2 = (x-cx) * memsin[ ith - 2700 ] + (y-cy) * - memsin[ 3600 - ith ] + cx;
		*y2 = (y-cy) * memsin[ ith - 2700 ] - (x-cx) * - memsin[ 3600 - ith ] + cy;
	}
	else if ( ( ith >= -900 ) && ( ith <= 0 ) ) {
		*x2 = (x-cx) * memsin[ 900 + ith ] + (y-cy) * - memsin[ - ith ] + cx;
		*y2 = (y-cy) * memsin[ 900 + ith ] - (x-cx) * - memsin[ - ith ] + cy;
	}
	else if ( ( ith >= -1800 ) && ( ith <= -900 ) ) {
		*x2 = (x-cx) * - memsin[ - ith - 900 ] + (y-cy) * - memsin[ 1800 + ith ] + cx;
		*y2 = (y-cy) * - memsin[ - ith - 900 ] - (x-cx) * - memsin[ 1800 + ith ] + cy;
	}
	else if ( ( ith >= -2700 ) && ( ith <= -1800 ) ) {
		*x2 = (x-cx) * - memsin[ 2700 + ith ] + (y-cy) * memsin[ - ith - 1800 ] + cx;
		*y2 = (y-cy) * - memsin[ 2700 + ith ] - (x-cx) * memsin[ - ith - 1800 ] + cy;
	}
	else if ( ( ith >= -3600 ) && ( ith <= -2700 ) ) {
		*x2 = (x-cx) * memsin[ - ith - 2700 ] + (y-cy) * memsin[ 3600 + ith ] + cx;
		*y2 = (y-cy) * memsin[ - ith - 2700 ] - (x-cx) * memsin[ 3600 + ith ] + cy;
	}
}
//
//
//
enum { DRAW_LINE , DRAW_POLYGON  , DRAW_CIRCLE , DRAW_TEXT };

#define IMAGE_OBJECT_SIZE		24
#define IMAGE_POINT_SIZE		20
int     g_nCount = 0;
int     g_bIncrease = TRUE;

typedef struct _IMAGE_TABLE {
	int			Type;
	int			Pointer;
} _IMAGE_TABLE;

typedef struct _IMAGE_CIRCLE_TABLE {
	int			fcolor;
	int			ecolor;
	double		Point_X;
	double		Point_Y;
	double		Point_XS;
	double		Point_YS;
} _IMAGE_CIRCLE_TABLE;

typedef struct _IMAGE_LINE_TABLE {
	int			ecolor;
	double		Point_X1;
	double		Point_Y1;
	double		Point_X2;
	double		Point_Y2;
} _IMAGE_LINE_TABLE;

typedef struct _IMAGE_POLYGON_TABLE {
	int			fcolor;
	int			ecolor;
	int			len;
	double		Point_X[IMAGE_POINT_SIZE];
	double		Point_Y[IMAGE_POINT_SIZE];
} _IMAGE_POLYGON_TABLE;

typedef struct _IMAGE_TEXT_TABLE {
	int			tcolor;
	int			bcolor;
	double		Point_X;
	double		Point_Y;
	double		Point_XS;
	double		Point_YS;
	char        *format;
	int			size;
	int			datap;
	int			option;
} _IMAGE_TEXT_TABLE;

_IMAGE_LINE_TABLE	IMAGE_LINE_TABLE[] = {
		0 ,		0.0   , 0.0  , 0.0 , 0.0 
};

_IMAGE_TEXT_TABLE	IMAGE_TEXT_TABLE[] = {
		0   ,  0   ,  0.0  , -5.5 , -1.8 ,  1.2 , "%c" , 20 ,  1 , 'A' ,
		0   ,  0   ,  0.0  , -5.5 , -0.5 ,  1.2 , "%d" , 20 ,  0 ,  0  ,
		19  ,  0   ,  0.5  , -4.0 , -0.5 ,  0.5 , "%c" , 15 , -1 , 'A' ,
		19  ,  0   ,  0.5  , -4.0 , -0.5 ,  0.5 , "%c" , 15 , -1 , 'B' ,
};

_IMAGE_CIRCLE_TABLE	IMAGE_CIRCLE_TABLE[] = {
		42	,	16	,		0.0   ,  0.0 ,  3.3 , 3.3   ,

		12	,	57	,		0.0   ,  0.0 , 1.38 , 1.38  ,
		13	,	7	,		0.0   ,  0.0 , 1.0  , 1.0   ,
		12	,	57	,		-5.37 ,  0.0 , 1.0  , 1.0   , 

		12	,	57	,		0     ,  0   , 0.86 , 0.86  ,
		12	,	57	,		-5.37 ,  0   , 0.86 , 0.86  ,
		13	,	7	,		-5.37 ,  0   , 0.65 , 0.65  ,

		12	,	57	,		0	  ,  0   , 0.65 , 0.65  ,
		13	,	7	,		0     ,  0   , 0.5  , 0.5   ,

		108	,	110	,		0.0   ,  -5.5 ,  2.7,  2.7  ,

};


_IMAGE_POLYGON_TABLE	IMAGE_POLYGON_TABLE[] = {
		38 , 16 , 5 ,
            -5.37 ,   0.0 ,  0.0  , -5.37 , -5.37 , 0 , 0 , 0 , 0 , 0 ,	0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 ,
			-1.0  , -1.38 ,  1.38 ,  1.0  , -1.0  , 0 , 0 , 0 , 0 , 0 ,	0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 ,

		36 , 36 , 4 ,
            -4.18 , -4.18 , -1.5  , -1.5 , 0 , 0 , 0 , 0 , 0 , 0 ,	0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 ,
			 0.62 , -0.62 , -0.62 , 0.62 , 0 , 0 , 0 , 0 , 0 , 0 ,	0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 ,

		34 , 34 , 4 ,
            -3.9  , -3.9  ,  -1.8 , -1.8 , 0 , 0 , 0 , 0 , 0 , 0 ,	0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 ,
			 0.3  , -0.3  ,  -0.3 ,  0.3 , 0 , 0 , 0 , 0 , 0 , 0 ,	0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 ,

		37 , 57 , 4 ,
            -5.37 ,  -5.37  ,    0.0 , 0.0   , 0 , 0 , 0 , 0 , 0 , 0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 ,
			0.875 ,  -0.875 , -0.875 , 0.875 , 0 , 0 , 0 , 0 , 0 , 0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 ,

		39 , 55 , 4 ,
            -0.68 ,  -0.68 ,  0.68 ,  0.68 , 0 , 0 , 0 , 0 , 0 , 0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 ,
			-2.37 ,      0 ,     0 , -2.37 , 0 , 0 , 0 , 0 , 0 , 0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 ,

		13 , 16 , 10 ,
            -0.68 ,  -1.5  , -1.5 ,   -1 , -0.68 , 0.68  ,    1 , 1.5  , 1.5 , 0.68  , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 ,
			-2.37 ,  -3    , -7.5 , -7.5 , -7.15 , -7.15 , -7.5 , -7.5 , -3  , -2.37 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 ,

		107 , 107 , 11 ,
			 0.6792  , 0.9678  , 1.7642  , 1.6658 , 0.4082  , -1.0174 , -2.0547 , -2.0772 , -1.1816 , 0.4377  , 0.7275  , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
			-7.4514 , -6.3632 , -5.5403 , -4.235 , -3.4389 , -3.5616 , -4.9408 , -6.2946 , -7.3813 , -7.6209 , -7.4272 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,

		106 , 106 , 10 ,
			0.8441  , 1.0629  , 0.3385  , -0.0712 , -0.5536 , -1.4245 , -1.6678 , -1.1133 , -0.2196 , 0.9163  , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
			-4.1877 , -5.1302 , -5.5663 , -6.4854 , -7.2112 , -6.4388 , -5.2062 , -4.166  , -3.8264 , -3.9459 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,

		105 , 105 , 7 ,
			-0.5065 , -0.0487 , 0.192   , -1.0405 , -1.3776 , -1.2315 , -0.7478 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
			-6.1959 , -5.1558 , -4.4544 , -4.3835 , -5.3267 , -6.221  , -6.4863 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,

		108 , 110 , 4 , // 9
			 -2.7 , -2.7 ,  2.7 ,  2.7 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
			 -3.3 , -8.7 , -8.7 , -3.3 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
		108 , 110 , 4 , // 10 // Mask A
			 -2.9 , -2.9 ,  2.9 ,  2.9 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
			 -3.5 , -8.5 , -8.5 , -3.5 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
		108 , 110 , 4 , // 11 // Mask A
			 -2.5 , -2.5 ,  2.5 ,  2.5 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
			 -3.1 , -8.9 , -8.9 , -3.1 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
		108 , 110 , 4 , // 12 // Mask A
			 -3.1 , -3.1 ,  3.1 ,  3.1 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
			 -3.7 , -8.3 , -8.3 , -3.7 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
		108 , 110 , 4 , // 13 // Mask A
			 -2.3 , -2.3 ,  2.3 ,  2.3 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
			 -2.9 , -9.1 , -9.1 , -2.9 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
			 
//============================================================================			 
		39 , 55 , 6 ,
            -0.90  ,  -1.20 ,  -1.20 ,  1.20 ,  1.20 , 0.90 , 0 , 0 , 0 , 0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 ,
			 1.00  ,   0.00 ,  -2.00 , -2.00 ,  0.00 , 1.00 , 0 , 0 , 0 , 0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 ,

		13 , 16 , 9 ,
            -0.90 ,  -0.90 , -0.70 , -0.50 ,    0 ,  0.50 ,  0.70 ,  0.90 ,  0.90 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 ,
			-2.00 ,  -3.50 , -7.10 , -7.30 , -7.5 , -7.30 , -7.10 , -3.50 , -2.00 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 , 0.0 ,

};


_IMAGE_TABLE	IMAGE_TABLE[] = {
	//0 Full Rotate Plate
		DRAW_CIRCLE ,	0 ,
	//1 Arm 1(A)
		DRAW_POLYGON , 	0 ,
		DRAW_POLYGON , 	1 ,
		DRAW_POLYGON , 	2 ,
		DRAW_CIRCLE , 	1 ,
		DRAW_CIRCLE , 	2 ,
		DRAW_CIRCLE , 	3 ,
	//7 Arm 2(A)
		DRAW_POLYGON , 	3 ,
		DRAW_CIRCLE , 	4 ,
		DRAW_CIRCLE , 	5 ,
		DRAW_CIRCLE , 	6 ,
	//11 Fork (A)
		DRAW_POLYGON , 	4 ,
		DRAW_POLYGON , 	5 ,
		DRAW_CIRCLE , 	7 ,
		DRAW_CIRCLE , 	8 ,

	//2008.10.15
		DRAW_TEXT	 , 	2 , //15
		DRAW_TEXT	 , 	3 , //16
	//15 --> 17 Wafer(A)
		DRAW_CIRCLE , 	9 , // Wafer
		DRAW_POLYGON , 	9 , // Mask
		DRAW_POLYGON , 	6 ,
		DRAW_POLYGON ,	7 ,
		DRAW_POLYGON , 	8 ,
		DRAW_TEXT	 , 	0 ,
		DRAW_TEXT	 , 	1 ,	    
};

double Point_Buffer_x[IMAGE_OBJECT_SIZE][IMAGE_POINT_SIZE];
double Point_Buffer_y[IMAGE_OBJECT_SIZE][IMAGE_POINT_SIZE];

//-------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void DRAW_Text( HDC hDC , HPALETTE hPal , int x , int y , int size , char *str  , int tcolor) {
	HFONT   hOldFont;
	HFONT   hNewFont;
	RECT	wRect;
	int		len = lstrlen(str);

	SelectPalette  (hDC , hPal , 1);
	RealizePalette (hDC);
	SetBkMode      (hDC , TRANSPARENT);
    SetTextColor   (hDC , PALETTEINDEX(tcolor));

	hNewFont = CreateFont  (size , 0 , 0 , 0 , FW_BOLD , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , "Arial" );
	hOldFont = SelectObject(hDC  , hNewFont );

	wRect.left   = x;
	wRect.top    = y;
	wRect.right  = x + ((size/2) * len ) + 1;
	wRect.bottom = y + size;

	DrawTextEx  (hDC , str , -1 , &wRect , DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX , NULL);

	SelectObject(hDC, hOldFont);
	DeleteObject(hNewFont);
}
//
void DRAW_Line( HDC hDC , HPALETTE hPal , int x1 , int y1 , int x2 , int y2 , int lcolor ) {
	HPEN	hOldBrush;
	HPEN	hNewBrush;

	SelectPalette  (hDC , hPal , 1);
	RealizePalette (hDC );
	
	hNewBrush = CreatePen   (PS_SOLID , 1 , PALETTEINDEX(lcolor));
	hOldBrush = SelectObject(hDC , hNewBrush);
	MoveToEx(hDC , x1 , y1 , NULL);
	LineTo  (hDC , x2 , y2);
	hNewBrush = SelectObject(hDC , hOldBrush);
	DeleteObject(hNewBrush);
}
//
//
void DRAW_PolyGon( HDC hDC , HPALETTE hPal , POINT *PointArray , int len , int b , int l ) {
	HPEN	hOldBrush1;
	HPEN	hNewBrush1;
	HPEN	hOldBrush2;
	HPEN	hNewBrush2;

	SelectPalette ( hDC , hPal , 1 );
	RealizePalette ( hDC );
	if (b >= 0 && l >= 0) {
		hNewBrush1 = (HPEN) CreateSolidBrush(PALETTEINDEX(b));
		hOldBrush1 = SelectObject(hDC , hNewBrush1);
		hNewBrush2 = CreatePen(PS_SOLID , 1 , PALETTEINDEX(l));
		hOldBrush2 = SelectObject(hDC , hNewBrush2);
		Polygon(hDC , PointArray , len);
		hNewBrush1 = SelectObject(hDC , hOldBrush1);
		hNewBrush2 = SelectObject(hDC , hOldBrush2);
		DeleteObject(hNewBrush1);
		DeleteObject(hNewBrush2);
	}
	else if (b >= 0) {
		hNewBrush1 = (HPEN) CreateSolidBrush(PALETTEINDEX(b));
		hOldBrush1 = SelectObject(hDC , hNewBrush1);
		Polygon(hDC , PointArray , len);
		hNewBrush1 = SelectObject(hDC , hOldBrush1);
		DeleteObject(hNewBrush1);
	}
	else if (l >= 0) {
		hNewBrush2 = CreatePen(PS_SOLID , 1 , PALETTEINDEX(l));
		hOldBrush2 = SelectObject(hDC , hNewBrush2);
		Polyline(hDC , PointArray , len);
		hNewBrush2 = SelectObject(hDC , hOldBrush2);
		DeleteObject(hNewBrush2);
	}
}
//
void DRAW_Circle(HDC hDC , HPALETTE hPal , int left , int top , int right , int bottom , int b , int l) {
	HPEN	hOldBrush1;
	HPEN	hNewBrush1;
	HPEN	hOldBrush2;
	HPEN	hNewBrush2;

	SelectPalette  (hDC , hPal , 1);
	RealizePalette (hDC);

	hNewBrush1 = (HPEN) CreateSolidBrush(PALETTEINDEX(b));
	hOldBrush1 = SelectObject(hDC , hNewBrush1);
	hNewBrush2 = CreatePen(PS_SOLID , 1 , PALETTEINDEX(l));
	hOldBrush2 = SelectObject(hDC , hNewBrush2);
	
	Ellipse(hDC , left , top , right , bottom);

	hNewBrush1 = SelectObject(hDC , hOldBrush1);
	hNewBrush2 = SelectObject(hDC , hOldBrush2);
	DeleteObject(hNewBrush1);
	DeleteObject(hNewBrush2);
}
//----------------------------------------------------------------------------------
void IMAGE_DRAWING( HDC hDC , HPALETTE , int i , int m , int c1 , int c2 , double d , int xs , int ys , int l );
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
double		BACK_YARD = -30;
//----------------------------------------------------------------------------------
__declspec(dllexport) void IMAGE_LIBRARY_PALETTE( HDC hDC , HPALETTE hPal , int x , int y , int i , int ip1 , int ip2 , int ip3 , int ip4 , int ip5 , double d , double dp1 , double dp2 , double dp3 , char *s , char *sp1 , char *sp2 , char *sp3 ) {
	POINT	PointArray[IMAGE_POINT_SIZE];
	char Buffer[64];
	int j , k , rpmode , wmmode;
	double xo;
	double ebuffer1;
	double zoom2;

	//---------------------------------------------------------------
	// Index Map
	//===============================================================
	// i   - Wafer_Status
	// ip1 - Wafer_Source
	// ip2 - XXX
	//       TTT
	//       |T+-> Arm Side  (0:Left 1:Right)
	//       |+--> Arm Style (0:Default 1:Style1)
	//       +---> Arm Back  (0:-30 , 1:-27 , 2:-24 , 3:-21 , 4:-18 , 5:-15 , 6:-12 , 7:-9 , 8:-6 , 9:0)
	// ip3 - XX
	//       TT
	//       T+-> Rotate Plate Draw (0:Hide 1:Draw)
	//       +--> Wafer Mask Mode (0:Wafer 1:Mask)
	// ip4 - Wafer Result
	// ip5 - Wafer Size
	//---------------------------------------------------------------
	// d   - rotate_value
	// dp1 - extend_value
	// dp2 - x_move_value
	// dp3 - zoom
	//---------------------------------------------------------------
	// s   - N/A
	// sp1 - N/A
	// sp2 - N/A
	// sp3 - N/A
	//---------------------------------------------------------------

	if ( ip2 % 10 ) xo = -1;
	else   	        xo =  1;
	//
	switch( ( ip2 % 100 ) / 10 ) { // 2003.03.05
	case 0 :
		IMAGE_TABLE[11].Pointer = 4;
		IMAGE_TABLE[12].Pointer = 5;
		break;
	case 1 :
		IMAGE_TABLE[11].Pointer = 14;
		IMAGE_TABLE[12].Pointer = 15;
		break;
	}
	switch( ip2 / 100 ) { // 2003.03.05
	case 0 : BACK_YARD = -30; break;
	case 1 : BACK_YARD = -27; break;
	case 2 : BACK_YARD = -24; break;
	case 3 : BACK_YARD = -21; break;
	case 4 : BACK_YARD = -18; break;
	case 5 : BACK_YARD = -15; break;
	case 6 : BACK_YARD = -12; break;
	case 7 : BACK_YARD =  -9; break;
	case 8 : BACK_YARD =  -6; break;
	case 9 : BACK_YARD =   0; break;
	}
	// dp2 = m position
	if ( dp3 <= 1 ) dp3 = 1;

	rpmode   = ip3 % 10;
	wmmode   = ip3 / 10;
	ebuffer1 = (dp1+BACK_YARD)*xo;
	zoom2    = dp3 + ((double) ip5 / 10 );

	switch (wmmode) {
	case 2 :
		IMAGE_TABLE[18].Pointer = 10;
 		wmmode = 1;
		break;
	case 3 :
		IMAGE_TABLE[18].Pointer = 11;
		wmmode = 1;
		break;
	case 4 :
		IMAGE_TABLE[18].Pointer = 12;
		wmmode = 1;
		break;
	case 5 :
		IMAGE_TABLE[18].Pointer = 13;
		wmmode = 1;
		break;
	}


	//2008.10.15
	if (1 == ip2) {
		//Arm B
		IMAGE_CIRCLE_TABLE[1]  .fcolor = 12 + g_nCount;
		IMAGE_CIRCLE_TABLE[2]  .fcolor = 13 + g_nCount;
		IMAGE_CIRCLE_TABLE[3]  .fcolor = 12 + g_nCount;
		IMAGE_CIRCLE_TABLE[4]  .fcolor = 12 + g_nCount;
		IMAGE_CIRCLE_TABLE[5]  .fcolor = 12 + g_nCount;
		IMAGE_CIRCLE_TABLE[6]  .fcolor = 13 + g_nCount;
		IMAGE_CIRCLE_TABLE[7]  .fcolor = 12 + g_nCount;
		IMAGE_CIRCLE_TABLE[8]  .fcolor = 13 + g_nCount;
		IMAGE_POLYGON_TABLE[0] .fcolor = 41 + g_nCount;
		IMAGE_POLYGON_TABLE[1] .fcolor = 39 + g_nCount;
		IMAGE_POLYGON_TABLE[2] .fcolor = 37 + g_nCount;
		IMAGE_POLYGON_TABLE[3] .fcolor = 40 + g_nCount;
		IMAGE_POLYGON_TABLE[4] .fcolor = 42 + g_nCount;
		IMAGE_POLYGON_TABLE[5] .fcolor = 13 + g_nCount;

		if      (g_nCount > 3) {
			g_nCount -= 1;
			g_bIncrease = FALSE;
		} else if (g_nCount < 0) {
			g_nCount += 1;
			g_bIncrease = TRUE;
		} else {
			if (g_bIncrease) g_nCount += 1;
			else             g_nCount -= 1;
		}
	}
	//2008.10.15

	if (ip4 == 99) {
		if (ip1 >= 4) {
			IMAGE_CIRCLE_TABLE[9]  .fcolor = 162;
			IMAGE_POLYGON_TABLE[6] .fcolor = 161;
			IMAGE_POLYGON_TABLE[7] .fcolor = 160;
			IMAGE_POLYGON_TABLE[8] .fcolor = 159;
			IMAGE_POLYGON_TABLE[9] .fcolor = 162;
			IMAGE_POLYGON_TABLE[10].fcolor = 162;
			IMAGE_POLYGON_TABLE[11].fcolor = 162;
			IMAGE_POLYGON_TABLE[12].fcolor = 162;
			IMAGE_POLYGON_TABLE[13].fcolor = 162;
			IMAGE_CIRCLE_TABLE[9]  .ecolor = 163;
			IMAGE_POLYGON_TABLE[6] .ecolor = 161;
			IMAGE_POLYGON_TABLE[7] .ecolor = 160;
			IMAGE_POLYGON_TABLE[8] .ecolor = 159;
			IMAGE_POLYGON_TABLE[9] .ecolor = 163;
			IMAGE_POLYGON_TABLE[10].ecolor = 163;
			IMAGE_POLYGON_TABLE[11].ecolor = 163;
			IMAGE_POLYGON_TABLE[12].ecolor = 163;
			IMAGE_POLYGON_TABLE[13].ecolor = 163;
		}
		else {
			IMAGE_CIRCLE_TABLE[9]  .fcolor = 118;
			IMAGE_POLYGON_TABLE[6] .fcolor = 117;
			IMAGE_POLYGON_TABLE[7] .fcolor = 116;
			IMAGE_POLYGON_TABLE[8] .fcolor = 115;
			IMAGE_POLYGON_TABLE[9] .fcolor = 118;
			IMAGE_POLYGON_TABLE[10].fcolor = 118;
			IMAGE_POLYGON_TABLE[11].fcolor = 118;
			IMAGE_POLYGON_TABLE[12].fcolor = 118;
			IMAGE_POLYGON_TABLE[13].fcolor = 118;
			IMAGE_CIRCLE_TABLE[9]  .ecolor = 120;
			IMAGE_POLYGON_TABLE[6] .ecolor = 117;
			IMAGE_POLYGON_TABLE[7] .ecolor = 116;
			IMAGE_POLYGON_TABLE[8] .ecolor = 115;
			IMAGE_POLYGON_TABLE[9] .ecolor = 120;
			IMAGE_POLYGON_TABLE[10].ecolor = 120;
			IMAGE_POLYGON_TABLE[11].ecolor = 120;
			IMAGE_POLYGON_TABLE[12].ecolor = 120;
			IMAGE_POLYGON_TABLE[13].ecolor = 120;
		}
	}
	else {
		switch( ip4 ) {
		case 1 :
			IMAGE_CIRCLE_TABLE[9]  .fcolor = 118;
			IMAGE_POLYGON_TABLE[6] .fcolor = 117;
			IMAGE_POLYGON_TABLE[7] .fcolor = 116;
			IMAGE_POLYGON_TABLE[8] .fcolor = 115;
			IMAGE_POLYGON_TABLE[9] .fcolor = 118;
			IMAGE_POLYGON_TABLE[10].fcolor = 118;
			IMAGE_POLYGON_TABLE[11].fcolor = 118;
			IMAGE_POLYGON_TABLE[12].fcolor = 118;
			IMAGE_POLYGON_TABLE[13].fcolor = 118;
			IMAGE_CIRCLE_TABLE[9]  .ecolor = 120;
			IMAGE_POLYGON_TABLE[6] .ecolor = 117;
			IMAGE_POLYGON_TABLE[7] .ecolor = 116;
			IMAGE_POLYGON_TABLE[8] .ecolor = 115;
			IMAGE_POLYGON_TABLE[9] .ecolor = 120;
			IMAGE_POLYGON_TABLE[10].ecolor = 120;
			IMAGE_POLYGON_TABLE[11].ecolor = 120;
			IMAGE_POLYGON_TABLE[12].ecolor = 120;
			IMAGE_POLYGON_TABLE[13].ecolor = 120;
			break;
		case 2 :
			IMAGE_CIRCLE_TABLE[9]  .fcolor = 78;
			IMAGE_POLYGON_TABLE[6] .fcolor = 77;
			IMAGE_POLYGON_TABLE[7] .fcolor = 76;
			IMAGE_POLYGON_TABLE[8] .fcolor = 75;
			IMAGE_POLYGON_TABLE[9] .fcolor = 78;
			IMAGE_POLYGON_TABLE[10].fcolor = 78;
			IMAGE_POLYGON_TABLE[11].fcolor = 78;
			IMAGE_POLYGON_TABLE[12].fcolor = 78;
			IMAGE_POLYGON_TABLE[13].fcolor = 78;
			IMAGE_CIRCLE_TABLE[9]  .ecolor = 80;
			IMAGE_POLYGON_TABLE[6] .ecolor = 77;
			IMAGE_POLYGON_TABLE[7] .ecolor = 76;
			IMAGE_POLYGON_TABLE[8] .ecolor = 75;
			IMAGE_POLYGON_TABLE[9] .ecolor = 80;
			IMAGE_POLYGON_TABLE[10].ecolor = 80;
			IMAGE_POLYGON_TABLE[11].ecolor = 80;
			IMAGE_POLYGON_TABLE[12].ecolor = 80;
			IMAGE_POLYGON_TABLE[13].ecolor = 80;
			break;
		default :
			IMAGE_CIRCLE_TABLE[9]  .fcolor = 108;
			IMAGE_POLYGON_TABLE[6] .fcolor = 107;
			IMAGE_POLYGON_TABLE[7] .fcolor = 106;
			IMAGE_POLYGON_TABLE[8] .fcolor = 105;
			IMAGE_POLYGON_TABLE[9] .fcolor = 108;
			IMAGE_POLYGON_TABLE[10].fcolor = 108;
			IMAGE_POLYGON_TABLE[11].fcolor = 108;
			IMAGE_POLYGON_TABLE[12].fcolor = 108;
			IMAGE_POLYGON_TABLE[13].fcolor = 108;
			IMAGE_CIRCLE_TABLE[9]  .ecolor = 110;
			IMAGE_POLYGON_TABLE[6] .ecolor = 107;
			IMAGE_POLYGON_TABLE[7] .ecolor = 106;
			IMAGE_POLYGON_TABLE[8] .ecolor = 105;
			IMAGE_POLYGON_TABLE[9] .ecolor = 110;
			IMAGE_POLYGON_TABLE[10].ecolor = 110;
			IMAGE_POLYGON_TABLE[11].ecolor = 110;
			IMAGE_POLYGON_TABLE[12].ecolor = 110;
			IMAGE_POLYGON_TABLE[13].ecolor = 110;
			break;
		}
	}

	//  Zoom Control
	for (j = 0 ; j < IMAGE_OBJECT_SIZE ; j++) {
		if (j == 0  && rpmode > 0 )       continue;
		if (j == 17 && wmmode != 0)       continue;
		if (j == 18 && wmmode != 1)       continue;
		if (j >= 17 && j <= 23 && i <= 0) continue;
		if (j >= 1  && j <= 10) {
			switch (IMAGE_TABLE[j].Type) {
			case DRAW_LINE :
				Point_Buffer_x[j][0] = IMAGE_LINE_TABLE[IMAGE_TABLE[j].Pointer].Point_X1 *  dp3 * xo;
				Point_Buffer_y[j][0] = IMAGE_LINE_TABLE[IMAGE_TABLE[j].Pointer].Point_Y1 * -dp3;
				Point_Buffer_x[j][1] = IMAGE_LINE_TABLE[IMAGE_TABLE[j].Pointer].Point_X2 *  dp3 * xo;
				Point_Buffer_y[j][1] = IMAGE_LINE_TABLE[IMAGE_TABLE[j].Pointer].Point_Y2 * -dp3;
				break;
			case DRAW_CIRCLE :
				Point_Buffer_x[j][0] = IMAGE_CIRCLE_TABLE[IMAGE_TABLE[j].Pointer].Point_X  *  dp3 * xo;
				Point_Buffer_y[j][0] = IMAGE_CIRCLE_TABLE[IMAGE_TABLE[j].Pointer].Point_Y  * -dp3;
				Point_Buffer_x[j][1] = IMAGE_CIRCLE_TABLE[IMAGE_TABLE[j].Pointer].Point_XS *  dp3;
				Point_Buffer_y[j][1] = IMAGE_CIRCLE_TABLE[IMAGE_TABLE[j].Pointer].Point_YS * -dp3;
				break;
			case DRAW_POLYGON :
				for (k = 0 ; k < IMAGE_POLYGON_TABLE[IMAGE_TABLE[j].Pointer].len ; k++) {
					Point_Buffer_x[j][k] = IMAGE_POLYGON_TABLE[IMAGE_TABLE[j].Pointer].Point_X[k] *  dp3 * xo;
					Point_Buffer_y[j][k] = IMAGE_POLYGON_TABLE[IMAGE_TABLE[j].Pointer].Point_Y[k] * -dp3;
				}
				break;
			case DRAW_TEXT :
				Point_Buffer_x[j][0] = IMAGE_TEXT_TABLE[IMAGE_TABLE[j].Pointer].Point_X  *  dp3 * xo;
				Point_Buffer_y[j][0] = IMAGE_TEXT_TABLE[IMAGE_TABLE[j].Pointer].Point_Y  * -dp3;
				Point_Buffer_x[j][1] = IMAGE_TEXT_TABLE[IMAGE_TABLE[j].Pointer].Point_XS *  dp3;
				Point_Buffer_y[j][1] = IMAGE_TEXT_TABLE[IMAGE_TABLE[j].Pointer].Point_YS * -dp3;
				break;
			}
		}
		else {
			if (j >= 17 && j <= 23) {
				switch (IMAGE_TABLE[j].Type) {
				case DRAW_LINE :
					Point_Buffer_x[j][0] = IMAGE_LINE_TABLE[IMAGE_TABLE[j].Pointer].Point_X1 *  zoom2;
					Point_Buffer_y[j][0] = IMAGE_LINE_TABLE[IMAGE_TABLE[j].Pointer].Point_Y1 * -zoom2;
					Point_Buffer_x[j][1] = IMAGE_LINE_TABLE[IMAGE_TABLE[j].Pointer].Point_X2 *  zoom2;
					Point_Buffer_y[j][1] = IMAGE_LINE_TABLE[IMAGE_TABLE[j].Pointer].Point_Y2 * -zoom2;
					break;
				case DRAW_CIRCLE :
					Point_Buffer_x[j][0] = IMAGE_CIRCLE_TABLE[IMAGE_TABLE[j].Pointer].Point_X  *  zoom2;
					Point_Buffer_y[j][0] = IMAGE_CIRCLE_TABLE[IMAGE_TABLE[j].Pointer].Point_Y  * -zoom2;
					Point_Buffer_x[j][1] = IMAGE_CIRCLE_TABLE[IMAGE_TABLE[j].Pointer].Point_XS *  zoom2;
					Point_Buffer_y[j][1] = IMAGE_CIRCLE_TABLE[IMAGE_TABLE[j].Pointer].Point_YS * -zoom2;
					break;
				case DRAW_POLYGON :
					for ( k = 0 ; k < IMAGE_POLYGON_TABLE[IMAGE_TABLE[j].Pointer].len ; k++ ) {
						Point_Buffer_x[j][k] = IMAGE_POLYGON_TABLE[IMAGE_TABLE[j].Pointer].Point_X[k] *  zoom2;
						Point_Buffer_y[j][k] = IMAGE_POLYGON_TABLE[IMAGE_TABLE[j].Pointer].Point_Y[k] * -zoom2;
					}
					break;
				case DRAW_TEXT :
					Point_Buffer_x[j][0] = IMAGE_TEXT_TABLE[IMAGE_TABLE[j].Pointer].Point_X  *  zoom2;
					Point_Buffer_y[j][0] = IMAGE_TEXT_TABLE[IMAGE_TABLE[j].Pointer].Point_Y  * -zoom2;
					Point_Buffer_x[j][1] = IMAGE_TEXT_TABLE[IMAGE_TABLE[j].Pointer].Point_XS *  zoom2;
					Point_Buffer_y[j][1] = IMAGE_TEXT_TABLE[IMAGE_TABLE[j].Pointer].Point_YS * -zoom2;
					break;
				}
			}
			else {
				switch ( IMAGE_TABLE[j].Type ) {
				case DRAW_LINE :
					Point_Buffer_x[j][0] = IMAGE_LINE_TABLE[IMAGE_TABLE[j].Pointer].Point_X1 *  dp3;
					Point_Buffer_y[j][0] = IMAGE_LINE_TABLE[IMAGE_TABLE[j].Pointer].Point_Y1 * -dp3;
					Point_Buffer_x[j][1] = IMAGE_LINE_TABLE[IMAGE_TABLE[j].Pointer].Point_X2 *  dp3;
					Point_Buffer_y[j][1] = IMAGE_LINE_TABLE[IMAGE_TABLE[j].Pointer].Point_Y2 * -dp3;
					break;
				case DRAW_CIRCLE :
					Point_Buffer_x[j][0] = IMAGE_CIRCLE_TABLE[IMAGE_TABLE[j].Pointer].Point_X  *  dp3;
					Point_Buffer_y[j][0] = IMAGE_CIRCLE_TABLE[IMAGE_TABLE[j].Pointer].Point_Y  * -dp3;
					Point_Buffer_x[j][1] = IMAGE_CIRCLE_TABLE[IMAGE_TABLE[j].Pointer].Point_XS *  dp3;
					Point_Buffer_y[j][1] = IMAGE_CIRCLE_TABLE[IMAGE_TABLE[j].Pointer].Point_YS * -dp3;
					break;
				case DRAW_POLYGON :
					for ( k = 0 ; k < IMAGE_POLYGON_TABLE[IMAGE_TABLE[j].Pointer].len ; k++ ) {
						Point_Buffer_x[j][k] = IMAGE_POLYGON_TABLE[IMAGE_TABLE[j].Pointer].Point_X[k] *  dp3;
						Point_Buffer_y[j][k] = IMAGE_POLYGON_TABLE[IMAGE_TABLE[j].Pointer].Point_Y[k] * -dp3;
					}
					break;
				case DRAW_TEXT :
					//2008.10.15
					if (0 == ip2 && j == 16)  continue;
					if (1 == ip2 && j == 15)  continue;					
					Point_Buffer_x[j][0] = IMAGE_TEXT_TABLE[IMAGE_TABLE[j].Pointer].Point_X  *  dp3;
					Point_Buffer_y[j][0] = IMAGE_TEXT_TABLE[IMAGE_TABLE[j].Pointer].Point_Y  * -dp3;
					Point_Buffer_x[j][1] = IMAGE_TEXT_TABLE[IMAGE_TABLE[j].Pointer].Point_XS *  dp3;
					Point_Buffer_y[j][1] = IMAGE_TEXT_TABLE[IMAGE_TABLE[j].Pointer].Point_YS * -dp3;					
					break;
				}
			}
		}
	}

	for (j = 11 ; j < IMAGE_OBJECT_SIZE ; j++) {
		if (j == 17 && wmmode != 0)       continue;
		if (j == 18 && wmmode != 1)       continue;
		if (j >= 17 && j <= 23 && i <= 0) continue;
		switch (IMAGE_TABLE[j].Type) {
			case DRAW_LINE :
				rotate( Point_Buffer_x[j][0] , Point_Buffer_y[j][0] , 0 , 0 , ebuffer1 , &Point_Buffer_x[j][0] , &Point_Buffer_y[j][0] );
				rotate( Point_Buffer_x[j][1] , Point_Buffer_y[j][1] , 0 , 0 , ebuffer1 , &Point_Buffer_x[j][1] , &Point_Buffer_y[j][1] );
				break;
			case DRAW_CIRCLE :
				rotate( Point_Buffer_x[j][0] , Point_Buffer_y[j][0] , 0 , 0 , ebuffer1 , &Point_Buffer_x[j][0] , &Point_Buffer_y[j][0] );
				break;
			case DRAW_POLYGON :
				for ( k = 0 ; k < IMAGE_POLYGON_TABLE[IMAGE_TABLE[j].Pointer].len ; k++ ) {
					rotate( Point_Buffer_x[j][k] , Point_Buffer_y[j][k] , 0 , 0 , ebuffer1 , &Point_Buffer_x[j][k] , &Point_Buffer_y[j][k] );
				}
				break;
			case DRAW_TEXT :
				//2008.10.15
				if (0 == ip2 && j == 16)  continue;
				if (1 == ip2 && j == 15)  continue;

				rotate( Point_Buffer_x[j][0] , Point_Buffer_y[j][0] , 0 , 0 , ebuffer1 , &Point_Buffer_x[j][0] , &Point_Buffer_y[j][0] );
				break;
		}
	}

	for (j = 7 ; j < IMAGE_OBJECT_SIZE ; j++) {
		if (j == 17 && wmmode != 0)       continue;
		if (j == 18 && wmmode != 1)       continue;
		if (j >= 17 && j <= 23 && i <= 0) continue;
		switch (IMAGE_TABLE[j].Type) {
			case DRAW_LINE :
				rotate( Point_Buffer_x[j][0] , Point_Buffer_y[j][0] , (-5.37*xo)*dp3 , 0 , -2*ebuffer1 , &Point_Buffer_x[j][0] , &Point_Buffer_y[j][0] );
				rotate( Point_Buffer_x[j][1] , Point_Buffer_y[j][1] , (-5.37*xo)*dp3 , 0 , -2*ebuffer1 , &Point_Buffer_x[j][1] , &Point_Buffer_y[j][1] );
				break;
			case DRAW_CIRCLE :
				rotate( Point_Buffer_x[j][0] , Point_Buffer_y[j][0] , (-5.37*xo)*dp3 , 0 , -2*ebuffer1 , &Point_Buffer_x[j][0] , &Point_Buffer_y[j][0] );
				break;
			case DRAW_POLYGON :
				for ( k = 0 ; k < IMAGE_POLYGON_TABLE[IMAGE_TABLE[j].Pointer].len ; k++ ) {
					rotate( Point_Buffer_x[j][k] , Point_Buffer_y[j][k] , (-5.37*xo)*dp3 , 0 , -2*ebuffer1 , &Point_Buffer_x[j][k] , &Point_Buffer_y[j][k] );
				}
				break;
			case DRAW_TEXT :
				//2008.10.15
				if (0 == ip2 && j == 16)  continue;
				if (1 == ip2 && j == 15)  continue;

				rotate( Point_Buffer_x[j][0] , Point_Buffer_y[j][0] , (-5.37*xo)*dp3 , 0 , -2*ebuffer1 , &Point_Buffer_x[j][0] , &Point_Buffer_y[j][0] );
				break;
		}
	}

	for (j = 1 ; j < IMAGE_OBJECT_SIZE ; j++) {
		if (j == 17 && wmmode != 0)       continue;
		if (j == 18 && wmmode != 1)       continue;
		if (j >= 17 && j <= 23 && i <= 0) continue;
		switch ( IMAGE_TABLE[j].Type ) {
			case DRAW_LINE :
				rotate( Point_Buffer_x[j][0] , Point_Buffer_y[j][0] , 0 , 0 , ebuffer1 , &Point_Buffer_x[j][0] , &Point_Buffer_y[j][0] );
				rotate( Point_Buffer_x[j][1] , Point_Buffer_y[j][1] , 0 , 0 , ebuffer1 , &Point_Buffer_x[j][1] , &Point_Buffer_y[j][1] );
				break;
			case DRAW_CIRCLE :
				rotate( Point_Buffer_x[j][0] , Point_Buffer_y[j][0] , 0 , 0 , ebuffer1 , &Point_Buffer_x[j][0] , &Point_Buffer_y[j][0] );
				break;
			case DRAW_POLYGON :
				for ( k = 0 ; k < IMAGE_POLYGON_TABLE[IMAGE_TABLE[j].Pointer].len ; k++ ) {
					rotate( Point_Buffer_x[j][k] , Point_Buffer_y[j][k] , 0 , 0 , ebuffer1 , &Point_Buffer_x[j][k] , &Point_Buffer_y[j][k] );
				}
				break;
			case DRAW_TEXT :
				//2008.10.15
				if (0 == ip2 && j == 16)  continue;
				if (1 == ip2 && j == 15)  continue;

				rotate( Point_Buffer_x[j][0] , Point_Buffer_y[j][0] , 0 , 0 , ebuffer1 , &Point_Buffer_x[j][0] , &Point_Buffer_y[j][0] );
				break;
		}
	}

	for (j = 0 ; j < IMAGE_OBJECT_SIZE ; j++) {
		if (j == 0 && rpmode > 0)         continue;
		if (j == 17 && wmmode != 0)       continue;
		if (j == 18 && wmmode != 1)       continue;
		if (j >= 17 && j <= 23 && i <= 0) continue;
		switch ( IMAGE_TABLE[j].Type ) {
			case DRAW_LINE :
				rotate( Point_Buffer_x[j][0] , Point_Buffer_y[j][0] , 0 , 0 , d , &Point_Buffer_x[j][0] , &Point_Buffer_y[j][0] );
				rotate( Point_Buffer_x[j][1] , Point_Buffer_y[j][1] , 0 , 0 , d , &Point_Buffer_x[j][1] , &Point_Buffer_y[j][1] );
				break;
			case DRAW_CIRCLE :
				rotate( Point_Buffer_x[j][0] , Point_Buffer_y[j][0] , 0 , 0 , d , &Point_Buffer_x[j][0] , &Point_Buffer_y[j][0] );
				break;
			case DRAW_POLYGON :
				for ( k = 0 ; k < IMAGE_POLYGON_TABLE[IMAGE_TABLE[j].Pointer].len ; k++ ) {
					rotate( Point_Buffer_x[j][k] , Point_Buffer_y[j][k] , 0 , 0 , d , &Point_Buffer_x[j][k] , &Point_Buffer_y[j][k] );
				}
				break;
			case DRAW_TEXT :
				//2008.10.15
				if (0 == ip2 && j == 16)  continue;
				if (1 == ip2 && j == 15)  continue;

				rotate( Point_Buffer_x[j][0] , Point_Buffer_y[j][0] , 0 , 0 , d , &Point_Buffer_x[j][0] , &Point_Buffer_y[j][0] );
				break;
		}
	}


	for (j = 0 ; j < IMAGE_OBJECT_SIZE ; j++) {
		if (j ==  0 && rpmode > 0)        continue;
		if (j == 22 && ip1 < 0)           continue;
		if (j == 17 && wmmode != 0)       continue;
		if (j == 18 && wmmode != 1)       continue;
		if (j >= 17 && j <= 23 && i <= 0) continue;
		switch( IMAGE_TABLE[j].Type ) {
			case DRAW_LINE :
				if (d > 90 && d < 270) {
					if (0 == ip2)
						DRAW_Line( hDC , hPal , (int) Point_Buffer_x[j][0] + x + (int) dp2 + 20 , (int) Point_Buffer_y[j][0] + y , (int) Point_Buffer_x[j][1] + x + (int) dp2+20 , (int) Point_Buffer_y[j][1] + y , IMAGE_LINE_TABLE[IMAGE_TABLE[j].Pointer].ecolor );
					else
						DRAW_Line( hDC , hPal , (int) Point_Buffer_x[j][0] + x + (int) dp2 - 20 , (int) Point_Buffer_y[j][0] + y , (int) Point_Buffer_x[j][1] + x + (int) dp2-20 , (int) Point_Buffer_y[j][1] + y , IMAGE_LINE_TABLE[IMAGE_TABLE[j].Pointer].ecolor );
				} else {
					DRAW_Line( hDC , hPal , (int) Point_Buffer_x[j][0] + x + (int) dp2 , (int) Point_Buffer_y[j][0] + y , (int) Point_Buffer_x[j][1] + x + (int) dp2 , (int) Point_Buffer_y[j][1] + y , IMAGE_LINE_TABLE[IMAGE_TABLE[j].Pointer].ecolor );
				}
				break;

			case DRAW_CIRCLE :
				if (d > 90 && d < 270) {
					if (0 == ip2)
						DRAW_Circle( hDC , hPal , (int) (Point_Buffer_x[j][0] - Point_Buffer_x[j][1]) + x + (int) dp2 + 20 , (int) (Point_Buffer_y[j][0] - Point_Buffer_y[j][1]) + y , (int) (Point_Buffer_x[j][0] + Point_Buffer_x[j][1]) + x + (int) dp2 + 20 , (int) (Point_Buffer_y[j][0] + Point_Buffer_y[j][1]) + y , IMAGE_CIRCLE_TABLE[IMAGE_TABLE[j].Pointer].fcolor , IMAGE_CIRCLE_TABLE[IMAGE_TABLE[j].Pointer].ecolor );
					else
						DRAW_Circle( hDC , hPal , (int) (Point_Buffer_x[j][0] - Point_Buffer_x[j][1]) + x + (int) dp2 - 20 , (int) (Point_Buffer_y[j][0] - Point_Buffer_y[j][1]) + y , (int) (Point_Buffer_x[j][0] + Point_Buffer_x[j][1]) + x + (int) dp2 - 20 , (int) (Point_Buffer_y[j][0] + Point_Buffer_y[j][1]) + y , IMAGE_CIRCLE_TABLE[IMAGE_TABLE[j].Pointer].fcolor , IMAGE_CIRCLE_TABLE[IMAGE_TABLE[j].Pointer].ecolor );
				} else {
					DRAW_Circle( hDC , hPal , (int) (Point_Buffer_x[j][0] - Point_Buffer_x[j][1]) + x + (int) dp2 , (int) (Point_Buffer_y[j][0] - Point_Buffer_y[j][1]) + y , (int) (Point_Buffer_x[j][0] + Point_Buffer_x[j][1]) + x + (int) dp2 , (int) (Point_Buffer_y[j][0] + Point_Buffer_y[j][1]) + y , IMAGE_CIRCLE_TABLE[IMAGE_TABLE[j].Pointer].fcolor , IMAGE_CIRCLE_TABLE[IMAGE_TABLE[j].Pointer].ecolor );
				}
				break;

			case DRAW_POLYGON :
				for ( k = 0 ; k < IMAGE_POLYGON_TABLE[IMAGE_TABLE[j].Pointer].len ; k++ ) {
					if (d > 90 && d < 270) {
						if (0 == ip2) {
							PointArray[k].x = (int) Point_Buffer_x[j][k] + x + (int) dp2 + 20;
							PointArray[k].y = (int) Point_Buffer_y[j][k] + y;
						} else {
							PointArray[k].x = (int) Point_Buffer_x[j][k] + x + (int) dp2 - 20;
							PointArray[k].y = (int) Point_Buffer_y[j][k] + y;
						}
					} else {
						PointArray[k].x = (int) Point_Buffer_x[j][k] + x + (int) dp2;
						PointArray[k].y = (int) Point_Buffer_y[j][k] + y;
					}
				}
				DRAW_PolyGon( hDC , hPal , PointArray , IMAGE_POLYGON_TABLE[IMAGE_TABLE[j].Pointer].len , IMAGE_POLYGON_TABLE[IMAGE_TABLE[j].Pointer].fcolor , IMAGE_POLYGON_TABLE[IMAGE_TABLE[j].Pointer].ecolor );
				break;
			
			case DRAW_TEXT :
				//2008.10.15
				if (0 == ip2 && j == 16)  continue;
				if (1 == ip2 && j == 15)  continue;

				switch( IMAGE_TEXT_TABLE[IMAGE_TABLE[j].Pointer].datap ) {
				case -1 : sprintf( Buffer , IMAGE_TEXT_TABLE[IMAGE_TABLE[j].Pointer].format , IMAGE_TEXT_TABLE[IMAGE_TABLE[j].Pointer].option ); break;
				case 0  : sprintf( Buffer , IMAGE_TEXT_TABLE[IMAGE_TABLE[j].Pointer].format , i + IMAGE_TEXT_TABLE[IMAGE_TABLE[j].Pointer].option ); break;
				case 1  : {
					//mgsong
					if (ip1 >= 5)
						ip1 = 3;
					
					sprintf( Buffer , IMAGE_TEXT_TABLE[IMAGE_TABLE[j].Pointer].format , ip1 + IMAGE_TEXT_TABLE[IMAGE_TABLE[j].Pointer].option ); break;
				}
			//	case 1  : sprintf( Buffer , IMAGE_TEXT_TABLE[IMAGE_TABLE[j].Pointer].format , ip1 + IMAGE_TEXT_TABLE[IMAGE_TABLE[j].Pointer].option ); break;
				case 2  : sprintf( Buffer , IMAGE_TEXT_TABLE[IMAGE_TABLE[j].Pointer].format , ip2 + IMAGE_TEXT_TABLE[IMAGE_TABLE[j].Pointer].option ); break;
				case 3  : sprintf( Buffer , IMAGE_TEXT_TABLE[IMAGE_TABLE[j].Pointer].format , ip3 + IMAGE_TEXT_TABLE[IMAGE_TABLE[j].Pointer].option ); break;
				case 4  : sprintf( Buffer , IMAGE_TEXT_TABLE[IMAGE_TABLE[j].Pointer].format , ip4 + IMAGE_TEXT_TABLE[IMAGE_TABLE[j].Pointer].option ); break;
				case 5  : sprintf( Buffer , IMAGE_TEXT_TABLE[IMAGE_TABLE[j].Pointer].format , ip5 + IMAGE_TEXT_TABLE[IMAGE_TABLE[j].Pointer].option ); break;
				case 6  : sprintf( Buffer , IMAGE_TEXT_TABLE[IMAGE_TABLE[j].Pointer].format , d + IMAGE_TEXT_TABLE[IMAGE_TABLE[j].Pointer].option ); break;
				case 7  : sprintf( Buffer , IMAGE_TEXT_TABLE[IMAGE_TABLE[j].Pointer].format , dp1 + IMAGE_TEXT_TABLE[IMAGE_TABLE[j].Pointer].option ); break;
				case 8  : 
					if (d > 90 && d < 270) {
						if (0 == ip2) {
							sprintf( Buffer , IMAGE_TEXT_TABLE[IMAGE_TABLE[j].Pointer].format , dp2 + 20 + IMAGE_TEXT_TABLE[IMAGE_TABLE[j].Pointer].option ); break;
						} else {
							sprintf( Buffer , IMAGE_TEXT_TABLE[IMAGE_TABLE[j].Pointer].format , dp2 - 20 + IMAGE_TEXT_TABLE[IMAGE_TABLE[j].Pointer].option ); break;							
						}
					} else {
						sprintf( Buffer , IMAGE_TEXT_TABLE[IMAGE_TABLE[j].Pointer].format , dp2 + IMAGE_TEXT_TABLE[IMAGE_TABLE[j].Pointer].option ); break;
					}
				case 9  : sprintf( Buffer , IMAGE_TEXT_TABLE[IMAGE_TABLE[j].Pointer].format , dp3 + IMAGE_TEXT_TABLE[IMAGE_TABLE[j].Pointer].option ); break;
				case 10 : sprintf( Buffer , IMAGE_TEXT_TABLE[IMAGE_TABLE[j].Pointer].format , s ); break;
				case 11 : sprintf( Buffer , IMAGE_TEXT_TABLE[IMAGE_TABLE[j].Pointer].format , sp1 ); break;
				case 12 : sprintf( Buffer , IMAGE_TEXT_TABLE[IMAGE_TABLE[j].Pointer].format , sp2 ); break;
				case 13 : sprintf( Buffer , IMAGE_TEXT_TABLE[IMAGE_TABLE[j].Pointer].format , sp3 ); break;
				}

				if (d > 90 && d < 270) {
					if (0 == ip2) {
							DRAW_Text( hDC , hPal , (int) (Point_Buffer_x[j][0] + Point_Buffer_x[j][1]) + x + (int) dp2 + 20 , (int) (Point_Buffer_y[j][0] + Point_Buffer_y[j][1]) + y , IMAGE_TEXT_TABLE[IMAGE_TABLE[j].Pointer].size , Buffer , IMAGE_TEXT_TABLE[IMAGE_TABLE[j].Pointer].tcolor);
					} else {
							DRAW_Text( hDC , hPal , (int) (Point_Buffer_x[j][0] + Point_Buffer_x[j][1]) + x + (int) dp2 - 20 , (int) (Point_Buffer_y[j][0] + Point_Buffer_y[j][1]) + y , IMAGE_TEXT_TABLE[IMAGE_TABLE[j].Pointer].size , Buffer , IMAGE_TEXT_TABLE[IMAGE_TABLE[j].Pointer].tcolor);
					}
				} else {
					DRAW_Text( hDC , hPal , (int) (Point_Buffer_x[j][0] + Point_Buffer_x[j][1]) + x + (int) dp2 , (int) (Point_Buffer_y[j][0] + Point_Buffer_y[j][1]) + y , IMAGE_TEXT_TABLE[IMAGE_TABLE[j].Pointer].size , Buffer , IMAGE_TEXT_TABLE[IMAGE_TABLE[j].Pointer].tcolor);
				}
				break;
		}
	}
}
