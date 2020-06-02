// 'clN22-kernel' - OpenCL 'zold-score'-grinder
// v0.931-020620 (c)2019-2020 ~EM eetu@kkona.xyz
// "Calculating "startAt" with WG sizes > 62 skipped over a bunch of characters -> fixed & proved working with wgsizes of 128 & 256. Still to fix: WG sizes < 62 - currently omits a bunch of lower values."

#define H0 0x6a09e667
#define H1 0xbb67ae85
#define H2 0x3c6ef372
#define H3 0xa54ff53a
#define H4 0x510e527f
#define H5 0x9b05688c
#define H6 0x1f83d9ab
#define H7 0x5be0cd19

uint rotr(uint x, int n) { if (n < 32) return (x >> n) | (x << (32 - n));return x; }
uint ch(uint x, uint y, uint z) { return (x & y) ^ (~x & z); }
uint maj(uint x, uint y, uint z) { return (x & y) ^ (x & z) ^ (y & z); }
uint sigma0(uint x) { return rotr(x, 2) ^ rotr(x, 13) ^ rotr(x, 22); }
uint sigma1(uint x) { return rotr(x, 6) ^ rotr(x, 11) ^ rotr(x, 25); }
uint gamma0(uint x) { return rotr(x, 7) ^ rotr(x, 18) ^ (x >> 3); }
uint gamma1(uint x) { return rotr(x, 17) ^ rotr(x, 19) ^ (x >> 10); }

__kernel void kernel22(__global short* dBufIn, __global char* plain_key, __global char* dValidKey) {
	ushort t, msg_pad, current_pad, stop, ai, lsNCF, gidx, gidy, gidz, posWorkgroupFactor, cGX, cGY, cGZ, startAt, mCount, wgDiv;
	short length = 6, rlInLen = dBufIn[0], hId0 = dBufIn[1], hId1 = dBufIn[2], preWorkgroupFactor = dBufIn[3], WGsize = dBufIn[4], mmod, i;
	const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789#";
	gidx = get_global_id(0);
	gidy = get_global_id(1);
	gidz = get_global_id(2);
	cGX = 0; cGY = 0; cGZ = 0;
	if (preWorkgroupFactor < 0) {
		posWorkgroupFactor = (ushort)abs(preWorkgroupFactor);
		mCount = (16 / (posWorkgroupFactor * posWorkgroupFactor));
		wgDiv = mCount > 1 ? (ushort)abs(WGsize / mCount) : (WGsize / 4);
		for (ai = 0; ai < posWorkgroupFactor; ai++) { if (gidz >= 62) { gidz -= 62; cGZ++; } }
		for (ai = 0; ai < posWorkgroupFactor; ai++) { if (gidx >= 62) { gidx -= 62; cGX += posWorkgroupFactor; } }
		for (ai = 0; ai < posWorkgroupFactor; ai++) { if (gidy >= 62) { gidy -= 62; cGY += posWorkgroupFactor * posWorkgroupFactor; } }
		startAt = cGZ + cGX + cGY;
	}
	else {
		gidx += preWorkgroupFactor * WGsize;
		gidy += preWorkgroupFactor * WGsize;
		gidz += preWorkgroupFactor * WGsize;
		mCount = 62;
	}
	if (gidx > 61 || gidy > 61 || gidz > 61 || startAt > 61)
		return;
	if (hId0 == 0 && hId1 == 0) {
		for (ai = 0; ai < length; ai++)
			dValidKey[ai] = charset[62];
	}
	uint lPart[8], W[80], A, B, C, D, E, F, G, H, T1, T2, c, item, total;
	short inLen = rlInLen + length;
	char local_key[90];
#pragma unroll
	for (ai = 0; ai < rlInLen; ai++)
		local_key[ai] = plain_key[ai];
	local_key[rlInLen + 1] = charset[hId0];
	local_key[rlInLen + 2] = charset[hId1];
	local_key[rlInLen + 3] = charset[gidz];
	local_key[rlInLen + 4] = charset[gidy];
	local_key[rlInLen + 5] = charset[gidx];
	total = (inLen & 63) >= 56 ? 2 : 1 + inLen / 64;
	uint K[64] = {
0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2 };
#pragma unroll
	for (lsNCF = 0; lsNCF < mCount; lsNCF++) {
		local_key[rlInLen] = preWorkgroupFactor < 0 ? charset[startAt] : charset[lsNCF];
		msg_pad = 0; c = 0; lPart[0] = H0; lPart[1] = H1; lPart[2] = H2; lPart[3] = H3; lPart[4] = H4; lPart[5] = H5; lPart[6] = H6; lPart[7] = H7;
#pragma unroll
		for (item = 0; item < total; item++) {
			A = lPart[0]; B = lPart[1]; C = lPart[2]; D = lPart[3]; E = lPart[4]; F = lPart[5]; G = lPart[6]; H = lPart[7];
#pragma unroll
			for (t = 0; t < 80; t++) { W[t] = 0x00000000; }
			msg_pad = item * 64;
			if (inLen > msg_pad) { current_pad = (inLen - msg_pad) > 64 ? 64 : (inLen - msg_pad); }
			else { current_pad = -1; }
			if (current_pad > 0) {
				i = current_pad;
				stop = i / 4;
#pragma unroll
				for (t = 0; t < stop; t++) {
					W[t] = ((uchar)local_key[msg_pad + t * 4]) << 24;
					W[t] |= ((uchar)local_key[msg_pad + t * 4 + 1]) << 16;
					W[t] |= ((uchar)local_key[msg_pad + t * 4 + 2]) << 8;
					W[t] |= (uchar)local_key[msg_pad + t * 4 + 3];
				}
				mmod = i < 0 ? i | ~3 : i & 3;
				if (mmod == 3) {
					W[t] = ((uchar)local_key[msg_pad + t * 4]) << 24;
					W[t] |= ((uchar)local_key[msg_pad + t * 4 + 1]) << 16;
					W[t] |= ((uchar)local_key[msg_pad + t * 4 + 2]) << 8;
					W[t] |= ((uchar)0x80);
				}
				else if (mmod == 2) {
					W[t] = ((uchar)local_key[msg_pad + t * 4]) << 24;
					W[t] |= ((uchar)local_key[msg_pad + t * 4 + 1]) << 16;
					W[t] |= 0x8000;
				}
				else if (mmod == 1) {
					W[t] = ((uchar)local_key[msg_pad + t * 4]) << 24;
					W[t] |= 0x800000;
				}
				else { W[t] = 0x80000000; }
				if (current_pad < 56) { W[15] = inLen * 8; }
			}
			else if (current_pad < 0) {
				if ((inLen & 63) == 0)
					W[0] = 0x80000000;
				W[15] = inLen * 8;
			}
#pragma unroll
			for (t = 0; t < 64; t++) {
				if (t >= 16) { W[t] = gamma1(W[t - 2]) + W[t - 7] + gamma0(W[t - 15]) + W[t - 16]; }
				T1 = H + sigma1(E) + ch(E, F, G) + K[t] + W[t];
				T2 = sigma0(A) + maj(A, B, C);
				H = G; G = F; F = E; E = D + T1; D = C; C = B; B = A; A = T1 + T2;
			}
			lPart[0] += A; lPart[1] += B; lPart[2] += C; lPart[3] += D; lPart[4] += E; lPart[5] += F; lPart[6] += G; lPart[7] += H;
		}
		uint v = lPart[7];
		if (v) {
			v &= -v;
			c = 0;
			if (v & 0xAAAAAAAAu) c |= 1;
			if (v & 0xCCCCCCCCu) c |= 2;
			if (v & 0xF0F0F0F0u) c |= 4;
			if (v & 0xFF00FF00u) c |= 8;
			if (v & 0xFFFF0000u) c |= 16;
		}
		else {
			for (ai = 0; ai < length; ai++) { dValidKey[ai] = local_key[rlInLen + ai]; }
			return;
		}
		if (preWorkgroupFactor < 0 && mCount > 1) {
			startAt += (wgDiv / posWorkgroupFactor);
			if (startAt > 61)
				return;
		}
	}
	return;
}
