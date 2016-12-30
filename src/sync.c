/* ctodo
 * Copyright (c) 2016 Niels Sonnich Poulsen (http://nielssp.dk)
 * Licensed under the MIT license.
 * See the LICENSE file or http://opensource.org/licenses/MIT for more information.
 */

#include <curl/curl.h>
#include <stdlib.h>
#include <string.h>

#include "sync.h"
#include "file.h"
#include "stream.h"
#include "error.h"

char *downloaded_file = NULL;
size_t downloaded_file_size = 0;

// TODO: implement stream_write and replace this
size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata) {
  size_t data_size = size * nmemb;
  if (downloaded_file) {
    downloaded_file = resize_buffer(downloaded_file, downloaded_file_size, downloaded_file_size + data_size);
  }
  else {
    downloaded_file = (char *)malloc(data_size);
    downloaded_file_size = 0;
  }
  if (!downloaded_file) {
    return 0;
  }
  memcpy(downloaded_file + downloaded_file_size, ptr, data_size);
  downloaded_file_size += data_size;
  return data_size;
}

TODOLIST *pull_todolist(char *origin) {
  TODOLIST *list = NULL;
  CURL *curl;
  CURLcode res;
  long http_status = 0;

  curl_global_init(CURL_GLOBAL_ALL);

  curl = curl_easy_init();
  if (curl) {
    curl_easy_setopt(curl, CURLOPT_URL, origin);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &write_callback);

    res = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_status);
    if (res != CURLE_OK) {
      error("Error: %s", curl_easy_strerror(res));
    }
    else if (http_status != 200) {
      error("Server returned %d", http_status);
    }
    else {
      list = parse_todolist(downloaded_file, downloaded_file_size);
    }
    if (downloaded_file) {
      free(downloaded_file);
      downloaded_file = NULL;
    }

    curl_easy_cleanup(curl);
  }
  curl_global_cleanup();
  return list;
}

int push_todolist(TODOLIST *todolist, char *origin) {
  CURL *curl;
  CURLcode res;
  char *content;
  STREAM *stream;
  long http_status = 0;
  int status = 1;

  curl_global_init(CURL_GLOBAL_ALL);

  curl = curl_easy_init();
  if (curl) {
    content = stringify_todolist(todolist);
    stream = stream_buffer(content, strlen(content));
    curl_easy_setopt(curl, CURLOPT_URL, origin);
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1);
    curl_easy_setopt(curl, CURLOPT_READDATA, stream);
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, &stream_read);

    res = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_status);
    if (res != CURLE_OK) {
      error("Error: %s", curl_easy_strerror(res));
      status = 0;
    }
    else if (http_status != 200) {
      error("Server returned %d", http_status);
      status = 0;
    }

    stream_close(stream);
    free(content);

    curl_easy_cleanup(curl);
  }
  curl_global_cleanup();
  return status;
}

int merge_todolist(TODOLIST *mine, TODOLIST *theirs) {
  return 1;
}
