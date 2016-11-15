#!/usr/bin/python
# -*- coding: utf-8 -*-
"""
Usage: 
python app.py <username>
"""
import concurrent.futures
import errno
import json
import os
import re
import requests
import tqdm
import sys
import warnings

warnings.filterwarnings("ignore")

class InstagramScraper:

    def __init__(self, username):
        self.username = username
        self.numPosts = 0
        self.executor = concurrent.futures.ThreadPoolExecutor(max_workers=10)
        self.future_to_item = {}

    def crawl(self, max_id=None):
        """Crawls through the user's media"""
        media = self.get_media(max_id)

        self.numPosts += len(media['items'])
        sys.stdout.write('\rFound %i post(s)' % self.numPosts)
        sys.stdout.flush()

        for item in media['items']:
            future = self.executor.submit(self.download, item, './' + self.username)
            self.future_to_item[future] = item

        if 'more_available' in media and media['more_available'] is True:
            max_id = media['items'][-1]['id']
            self.crawl(max_id)

    def get_media(self, max_id):
        """Gets the user's media metadata"""
        url = 'https://instagram.com/' + self.username + '/media'

        if max_id is not None:
            url += '?&max_id=' + max_id
        resp = requests.get(url)

        if resp.status_code == 200:
            media = json.loads(resp.text)

            if not media['items']:
                raise ValueError('User %s is private' % self.username)

            return media
        else:
            raise ValueError('User %s does not exist' % self.username)

    def download(self, item, save_dir='./'):
        """Downloads the media file"""
        try:
            os.makedirs(save_dir)
        except OSError as e:
            if e.errno == errno.EEXIST and os.path.isdir(save_dir):
                # another thread beat us to creating this dir
                pass
            else:
                # target dir exists as a file, or a different error
                raise

        item['url'] = item[item['type'] + 's']['standard_resolution']['url'].split('?')[0]
        # remove dimensions to get largest image
        item['url'] = re.sub(r'/s\d{3,}x\d{3,}/', '/', item['url']) 

        base_name = item['url'].split('/')[-1]
        file_path = os.path.join(save_dir, base_name)

        if not os.path.isfile(file_path):

            with open(file_path, 'wb') as file:
                try:
                    bytes = requests.get(item['url']).content
                except requests.exceptions.ConnectionError:
		    sleep(5)
		    bytes = requests.get(item['url']).content
					
                file.write(bytes)

            file_time = int(item['created_time'])
            os.utime(file_path, (file_time, file_time))

if __name__ == '__main__':
    username = sys.argv[1]

    scraper = InstagramScraper(username)
    scraper.crawl()

    for future in tqdm.tqdm(concurrent.futures.as_completed(scraper.future_to_item), total=len(scraper.future_to_item), desc='Downloading'):
        item = scraper.future_to_item[future]

        if future.exception() is not None:
            print('%r generated an exception: %s') % (item['id'], future.exception())
