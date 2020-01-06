
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <Windows.h>

#include "SR830-Driver.h" /* load original dirver file SR830-Driver.h, which need ni4882.lib in the same directory */

void foo(int Device, float minF, float maxF, float step, float *resF, float *resV)
{
	const int K_TC = 10; /* Ratio of waiting time and TIME_CONSTANT */
	FILE* outFile;

	char tstr[101], result[101];
	float TIME_CONSTANT = GetTimeConstant(Device);
	*resV = 0.0;
	SendMsg(Device, "ISRC0"); /* Set the input configuration to "A" port */
	Sleep(1000);
	printf("Testing(A port)...\n");
	
	fopen_s(&outFile, "result.txt", "w");
	fprintf(outFile, "F\tR\ttheta\t\n");
	for (float tF = 0.00005f; tF <= 0.01; tF += 0.00005f)
	{
		float F = 1.0f / (float)sqrt(tF);

		fclose(outFile);

		sprintf_s(tstr, 16, "FREQ%f", F); 
		SendMsg(Device, tstr);
		Sleep((DWORD)(TIME_CONSTANT * K_TC)); /* Wait for SR830 to get a reliable value */

		SendMsg(Device, "OUTP?3"); /* Query "R" data */
		ReadMsg(Device, result); /* Read "R" data */
		float R_A = (float)atof(result);

		Sleep((DWORD)(TIME_CONSTANT * K_TC)); /* Wait for SR830 to get a reliable value */
		SendMsg(Device, "OUTP?4"); /* Query "Theta" data */
		ReadMsg(Device, result); /* Read "Theta" data */
		float theta_A = (float)atof(result);

		fopen_s(&outFile, "result_A.txt", "a");
		printf("%.6f\t%.8f\t%.6f\t\n", F, R_A, theta_A);
		fprintf_s(outFile, "%.6f\t%.8f\t%.6f\t\n", F, R_A, theta_A);
		if (R_A > *resV)
		{
			*resV = R_A;
			*resF = F;
		}
	}
	fclose(outFile);

	printf("Testing finished.\n");
}


int main() 
{
	int Device = InitSR830(8, 0, 0); /* Initial SR830 device and get hangle */

	float resF, resV; /* Note that we had better use float instead of double variable */
	foo(Device, 10.0f, 200.0f, 5.0f, &resF, &resV); /* carry out sample task */
	printf("maxV = %f (when maxF = %f)", resV, resF); /* print sample output */

	ExitSR830(Device); /* Exit SR830 device */
}
