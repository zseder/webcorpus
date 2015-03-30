# -*- coding: utf-8 -*-

# Scrapy settings for webcrawl project
#
# For simplicity, this file contains only the most important settings by
# default. All the other settings are documented here:
#
#     http://doc.scrapy.org/en/latest/topics/settings.html
#

BOT_NAME = 'webcrawl'

SPIDER_MODULES = ['webcrawl.spiders']
NEWSPIDER_MODULE = 'webcrawl.spiders'
FEED_FORMAT = 'xml'
FEED_URI = 'dump.xml'
#SPIDER_MIDDLEWARES = { 'webcrawl.ignore.IgnoreVisitedItems': 560 }

CONCURRENT_REQUESTS = 200
LOG_LEVEL = 'INFO'
COOKIES_ENABLED = False
RETRY_ENABLED = False
DOWNLOAD_TIMEOUT = 15
REDIRECT_ENABLED = False

DEPTH_PRIORITY = 1
SCHEDULER_DISK_QUEUE = 'scrapy.squeue.PickleFifoDiskQueue'
SCHEDULER_MEMORY_QUEUE = 'scrapy.squeue.FifoMemoryQueue'

CONCURRENT_REQUESTS_PER_DOMAIN = 1
AUTOTHROTTLE_ENABLED = True

# Crawl responsibly by identifying yourself (and your website) on the user-agent
#USER_AGENT = 'webcrawl (+http://www.yourdomain.com)'
