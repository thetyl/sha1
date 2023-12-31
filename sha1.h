// MIT License
// 
// Copyright(c) 2023 Tyl
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software andassociated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, andto permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
// 
// The above copyright notice andthis permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <string>
#include <sstream>
#include <iomanip>

uint32_t circular_shift_left(uint32_t x, int count) {
	return (x << count) | (x >> (32 - count));
}

std::string sha1(const std::string &message) {
	uint32_t h[5];
	h[0] = 0x67452301;
	h[1] = 0xefcdab89;
	h[2] = 0x98badcfe;
	h[3] = 0x10325476;
	h[4] = 0xc3d2e1f0;

	uint32_t w[80];

	int index = 0;
	bool padded_one = false;
	bool lastChunk = false;

	while (!lastChunk) {
		for (int i = 0; i < 16; ++i) {
			w[i] = 0;

			for (int j = 0; j < 4; ++j) {
				if (index < message.size()) {
					w[i] |= message[index++] << ((3 - j) * 8);
				} else if (i < 14) {
					if (!padded_one) {
						w[i] |= 1 << (31 - j * 8);
						padded_one = true;
					}

					break;
				} else {
					if (!padded_one) {
						w[i] |= 1 << (31 - j * 8);
						padded_one = true;
					} else if (i == 14) {
						uint64_t message_size = static_cast<uint64_t>(message.size()) * 8;
						w[14] = message_size >> 32;
						w[15] = message_size & 0xffffffff;
						lastChunk = true;
					}

					break;
				}
			}

			if (lastChunk) {
				break;
			}
		}

		for (int i = 16; i < 80; ++i) {
			w[i] = circular_shift_left(w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16], 1);
		}

		uint32_t a = h[0];
		uint32_t b = h[1];
		uint32_t c = h[2];
		uint32_t d = h[3];
		uint32_t e = h[4];

		uint32_t f, k, temp;

		for (int i = 0; i < 80; ++i) {
			if (0 <= i && i <= 19) {
				f = (b & c) | ((~b) & d);
				k = 0x5a827999;
			} else if (20 <= i && i <= 39) {
				f = b ^ c ^ d;
				k = 0x6ed9eba1;
			} else if (40 <= i && i <= 59) {
				f = (b & c) | (b & d) | (c & d);
				k = 0x8f1bbcdc;
			} else {
				f = b ^ c ^ d;
				k = 0xca62c1d6;
			}

			temp = circular_shift_left(a, 5) + f + e + k + w[i];
			e = d;
			d = c;
			c = circular_shift_left(b, 30);
			b = a;
			a = temp;
		}

		h[0] += a;
		h[1] += b;
		h[2] += c;
		h[3] += d;
		h[4] += e;
	}

	std::stringstream ss;
	ss.fill('0');
	ss << std::hex;
	ss << std::setw(8) << h[0];
	ss << std::setw(8) << h[1];
	ss << std::setw(8) << h[2];
	ss << std::setw(8) << h[3];
	ss << std::setw(8) << h[4];

	return ss.str();
}
