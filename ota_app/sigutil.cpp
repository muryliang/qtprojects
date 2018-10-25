/*************************************************************************
	> File Name: createsig.c
	> Author: davee-x
	> Mail: davee.naughty@gmail.com 
	> Created Time: 2018年06月11日 星期一 21时29分38秒
 ************************************************************************/

// need to be careful of memory new and free when handle error

#include <cstdio>
#include <cstring>
#include <fstream>
#include <vector>

#ifdef __cplusplus
extern "C" {
#endif
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#ifdef __cplusplus
}
#endif

#include "sigutil.h"

#define READSIZ 1*1024*1024

#ifdef _DEBUG
#define pr_info(f_, ...) printf((f_), ##__VA_ARGS__)
#else
#define pr_info(f_, ...)
#endif

#define checkres(a,b,c) do { \
	if ((a) != (b)) { \
		pr_info(c); \
	} while(0)

#define checkresret(a,b,c,d) do { \
	if ((a) != (b)) { \
		pr_info(c); \
		return d; \
	} while(0)

#define checknres(a,b,c) do { \
	if ((a) != (b)) { \
		pr_info(c); \
	} while(0)

#define checknresret(a,b,c,d) do { \
	if ((a) != (b)) { \
		pr_info(c); \
		return d; \
	} while(0)

/*
 * readin a file content, calculate sha256sum
 * fname: input file name
 * result: output sha256sum result
 * return value:
 *		0 success
 *		-1 fail
 */
int get_sha256(const char *fname, unsigned char* result) {
	SHA256_CTX ctx;
	SHA256_Init(&ctx);

	int fd = open(fname, O_RDONLY);
	if (fd == -1) {
		pr_info("can not open file %s\n", fname);
		return -1;
	}

	unsigned char tmpbuf[READSIZ];
	int count = 0;
	while ((count = read(fd, tmpbuf, READSIZ)) > 0) {
		SHA256_Update(&ctx, tmpbuf, count);
	}
	SHA256_Final(result, &ctx);
	close(fd);
	return 0;
}

int generate_new_keypairs(const char *privkeypath, const char* pubkeypath) {

	int fd, count;
	EC_KEY *eckey;
	char *privkeystr = NULL , *pubkeystr = NULL;

	eckey = EC_KEY_new_by_curve_name(NID_secp256k1);
	if (eckey == NULL) {
		pr_info("can not create curve for eckey\n");
		return -1;
	}

	pr_info("begin generate key\n");
	if (!EC_KEY_generate_key(eckey)) {
		pr_info("cannot generate key from curve group\n");
		return -1;
	}

	pr_info("begin save privkey\n");
	// save priv key
	privkeystr = BN_bn2hex(EC_KEY_get0_private_key(eckey));
	if ((fd = open(privkeypath, O_WRONLY|O_CREAT|O_TRUNC, 0600)) == -1) {
		pr_info("can not open file for priv key store %s\n", privkeypath);
		return -1;
	}
	count = strlen((char*)privkeystr);
	if (write(fd, privkeystr, count) != count) {
		pr_info("error write to file of privkey\n");
		return -1;
	}
	free(privkeystr);
	close(fd);

	pr_info("begin save pubkey\n");
	// save pub key
	pubkeystr = EC_POINT_point2hex(EC_KEY_get0_group(eckey), EC_KEY_get0_public_key(eckey), POINT_CONVERSION_COMPRESSED, NULL);
	if ((fd = open(pubkeypath, O_WRONLY|O_CREAT|O_TRUNC, 0600)) == -1) {
		pr_info("can not open file for pub key store %s\n", pubkeypath);
		return -1;
	}
	count = strlen(pubkeystr);
	if (write(fd, pubkeystr, count) != count) {
		pr_info("error write to file of pubkey\n");
		return -1;
	}
	free(pubkeystr);
	close(fd);
	EC_KEY_free(eckey);
	return 0;
}

// caller should free buffer allocated inside this function
int readkeyfromfile(const char* fname, char** bufptr) {
	int fd, count;
	char *buf = NULL;
	// read privkey and sign
	if ((fd = open(fname, O_RDONLY)) == -1) {
		pr_info("can not open file %s for key read\n", fname);
		return -1;
	}
	count = lseek(fd, 0L, SEEK_END);
	lseek(fd, 0L, SEEK_SET);
	buf = (char*)malloc(count);
	if (buf == NULL) {
		pr_info("can not alloc mem for file %s key read\n", fname);
		return -1;
	}
	if (read(fd, buf, count) != count) {
		pr_info("can not read from file %s\n", fname);
		return -1;
	}
	*bufptr = buf;
	return count;
}

int readpriveckey(EC_KEY **priveckeyptr, const char *privkeypath, const char*pubkeypath) {
	EC_KEY *priveckey = NULL;
	EC_POINT *pubkey = NULL;
	char *keybuf = NULL;

	pr_info("begin read privkey\n");
	priveckey = EC_KEY_new_by_curve_name(NID_secp256k1);
	if (priveckey == NULL) {
		pr_info("can not create curve for priveckey\n");
		return -1;
	}

	// read privkey and sign
	if (readkeyfromfile(privkeypath, &keybuf) == -1) {
		pr_info("can not read privkey\n");
		return -1;
	}
	BIGNUM *privbignum = NULL;
	if (BN_hex2bn(&privbignum, keybuf) == 0) {
		pr_info("create big num fail\n");
		return 0;
	}
	free(keybuf);


	if (EC_KEY_set_private_key(priveckey, privbignum) != 1) {
		pr_info("can not set private key\n");
		return -1;
	}

	// also read pubkey, set pubkey into privkey
	if (readkeyfromfile(pubkeypath, &keybuf) == -1) {
		pr_info("can not read pubkey\n");
		return -1;
	}
	if ((pubkey = EC_POINT_hex2point(EC_KEY_get0_group(priveckey), keybuf, pubkey, NULL)) == NULL) {
		pr_info("can not convert from hex to ecpoint\n");
		return -1;
	}
	free(keybuf);

	if (EC_KEY_set_public_key(priveckey, pubkey) != 1) {
		pr_info("can not set public eckey into privkey\n");
		return -1;
	}

	*priveckeyptr = priveckey;
	return 0;
}

int readpubeckey(EC_KEY **pubeckeyptr, const char* pubkeypath) {
	EC_POINT *pubkey = NULL;
	EC_KEY *pubeckey = NULL;
	char *keybuf = NULL;

	// read pubkey and verify
	pr_info("begin read pubkey\n");
	pubeckey = EC_KEY_new_by_curve_name(NID_secp256k1);
	if (pubeckey == NULL) {
		pr_info("can not create curve for pubeckey\n");
		return -1;
	}
	// also read pubkey, set pubkey into privkey
	if (readkeyfromfile(pubkeypath, &keybuf) == -1) {
		pr_info("can not read pubkey\n");
		return -1;
	}
	if ((pubkey = EC_POINT_hex2point(EC_KEY_get0_group(pubeckey), keybuf, pubkey, NULL)) == NULL) {
		pr_info("can not convert from hex to ecpoint\n");
		return -1;
	}
	free(keybuf);

	if (EC_KEY_set_public_key(pubeckey, pubkey) != 1) {
		pr_info("can not set public eckey\n");
		return -1;
	}
	*pubeckeyptr = pubeckey;
	return 0;
}

/*
 * passin NULL siglen poiner and return size of signature needed
 *
 * if success, return 0
 *	  error, return -1
 */
int ecdsa_sign(unsigned char *content, int contentlen, unsigned char* sig, unsigned int *siglenptr, 
		const char *priveckeyname, const char *pubeckeyname) {

	EC_KEY *priveckey;

	// if private or public key file not exist, regenerate keys and store into file
	if (access((const char*)priveckeyname, F_OK) != 0 || access((const char*)pubeckeyname, F_OK) != 0) {
		if (generate_new_keypairs(priveckeyname, pubeckeyname) != 0) {
			pr_info("can not generate key pair\n");
			return -1;
		}
	}

	if (readpriveckey(&priveckey, priveckeyname, pubeckeyname) != 0) {
		pr_info("can not read priveckey\n");
		return -1;
	}

	// return size needed
	if (siglenptr == NULL || sig == NULL) {
		return ECDSA_size(priveckey);
	}

	//sign
	if (!ECDSA_sign(0, content, contentlen, sig, siglenptr, priveckey)) {
		pr_info("can not generate sig\n");
		return -1;
	}
	pr_info("sig length is %d, contentlen is %d\n", *siglenptr, contentlen);

	EC_KEY_free(priveckey);
	return 0;
}


/*
 * verify
 * return:
 *		-1 error, 0 verify fail, 1 success
 */
int ecdsa_verify(unsigned char *content, int contentlen, unsigned char* sig, unsigned int siglen, const char *pubeckeyname) {
	EC_KEY *pubeckey;
	int ret;

	// do a checkarg here
	if (sig == NULL) {
		pr_info("can not get sig\n");
		return -1;
	}

	// read public key
	if (readpubeckey(&pubeckey, pubeckeyname) != 0) {
		pr_info("can not read pubeckey\n");
		return -1;
	}

	// do verify
	ret = ECDSA_verify(0, content, contentlen, sig, siglen, pubeckey);
	if (ret == -1) {
		pr_info("verify error\n");
	} else if (ret == 0) {
		pr_info("verify fail, wrong sig\n");
	} else {
		pr_info("verify success\n");
	}
	EC_KEY_free(pubeckey);
	return ret;
}

void dumpbuf(const unsigned char* buf, int size) {
	for (int i = 0; i < size; ++i) {
		pr_info("%02x", buf[i]);
		if (i % 16 == 15) {
			putchar('\n');
		}
	}
}

// we preread pubkey into buffer ,so just passin the buffer here
bool checksig(const char * fname, const char * fsig, const char *pubkeypath) {
	unsigned char content[SHA256_DIGEST_LENGTH];
	int siglen;
	char *sig = NULL; //buffer to store signature
	int res = 0;
	bool ret = true;

	if (get_sha256(fname, content) == -1) {
		pr_info("can not get sha\n");
		ret = false;
		goto freenon;
	}

	if ((siglen = readkeyfromfile(fsig, &sig)) == -1) {
		pr_info("can not read sig from file %s\n", fname);
		ret = false;
		goto freenon;
	}
	
	res = ecdsa_verify(content, SHA256_DIGEST_LENGTH, (unsigned char*)sig, (unsigned int)siglen, pubkeypath);
	switch(res) {
	case -1:
	  pr_info("verify error occureed\n");
	  ret = false;
	  break;
	case 0:
	  pr_info("verify fail\n");
	  ret = false;
	  break;
	default:
	  pr_info("verify success file %s, can install\n", fname);
	  ret = true;
	  break;
	}

	free(sig);
freenon:
	return ret;

}

int do_copy_file(std::string from, std::string to) {
	unlink(to.c_str());
	std::ifstream ifs(from);
	std::ofstream ofs(to);
	if (!ifs || !ofs) {
		pr_info("do_copy_file can't get dir of copy file ifs or ofs\n");
		return -1;
	}
	pr_info("copy file from %s to %s\n", from.c_str(), to.c_str());
	ofs << ifs.rdbuf();	
	return 0;
}

int do_copy_pkg(std::string pkgpath, std::string installdir) {
	std::ifstream ifslist(pkgpath + "/FILELIST.lst");
	std::string pathline;
	if (!ifslist) {
		pr_info("can not copy package path %s\n", pkgpath.c_str());
		return -1;
	}
	while (std::getline(ifslist, pathline)) {
		if (pathline[pathline.length()-1] == '/') {
			std::string tmpdir = installdir + pathline;
			pr_info("makedir: %s%s\n", installdir.c_str(), pathline.c_str());
			mkdir(tmpdir.c_str(), 0775); // make dir, we use find(1) make sure dir created before copy file
		} else {
			do_copy_file(pkgpath + "/src/" + pathline, installdir + pathline);
		}
	}
	ifslist.close();
	return 0;
}

int uninstallpkg(std::string pkgpath, std::string installdir) {
	std::ifstream ifslist(pkgpath + "/FILELIST.lst");
    if (!ifslist) {
        pr_info("can not open file and uninstall, may be first install\n");
        return 0;
    }

	std::vector<std::string> reverselines;
	std::string pathline;
    while (std::getline(ifslist, pathline)) {
        if (pathline[pathline.length()-1] != '/') {
			std::string tmpdir = installdir + pathline;
            unlink(tmpdir.c_str());
			pr_info("unlink: %s\n", tmpdir.c_str());
        } else {
            reverselines.push_back(pathline); // push all dirs in reverse order
        }
    }

    // remove dirs if empty
    for (auto i = reverselines.rbegin(); i != reverselines.rend(); ++i) {
		std::string tmpdir = installdir + *i;
		pr_info("remove dir");
        rmdir(tmpdir.c_str());
    }
	return 0;
}

